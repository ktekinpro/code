#include "libudp.h"

/*--------------------------------Initialisation de la socket UDP coté client ---------------------------*/
int createSocket(char *hote,char *service, void** handle_res){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

	*handle_res = malloc(sizeof(struct addrinfo*));

	/* Création de l'adresse de socket */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	statut=getaddrinfo(hote,service,&precisions,&origine);
	if(statut<0){ perror("messageUDPgenerique.getaddrinfo");        exit(EXIT_FAILURE); }
	struct addrinfo *p;
	for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
	  if(p->ai_family==AF_INET6){ resultat=p; break; }

	/* Création d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("messageUDPgenerique.socket"); exit(EXIT_FAILURE); }

	/* Option sur la socket */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof(vrai))<0){
	  perror("initialisationServeurUDPgenerique.setsockopt (BROADCAST)");
	  exit(-1);
	  }

	*handle_res = resultat;

	/* Libération de la structure d'informations */
	freeaddrinfo(origine);

    return s;
}



/*------------------------------Client UDP-------------------------------------------------*/
void messageUDP(int s,struct addrinfo * handle_res,unsigned char *message,int taille){  // void *
	/* Envoi du message */
	int nboctets=sendto(s,message,taille,0,handle_res->ai_addr,handle_res->ai_addrlen);
	if(nboctets<0){ perror("messageUDPgenerique.sento"); exit(EXIT_FAILURE); }
}

/*--------------------------------Initialisation de la socket côté serveur---------------------------*/
int initialisationSocketUDP(char *service){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

	/* Construction de la structure adresse */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	precisions.ai_flags=AI_PASSIVE;
	statut=getaddrinfo(NULL,service,&precisions,&origine);
	if(statut<0){ perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }
	struct addrinfo *p;
	for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
	  if(p->ai_family==AF_INET6){ resultat=p; break; }

	/* Création d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

	/* Options utiles */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
	  perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
	  exit(-1);
	}

	/* Spécification de l'adresse de la socket */
	statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
	if(statut<0) {perror("initialisationServeurUDP.bind"); exit(-1);}

	/* Libération de la structure d'information */
	freeaddrinfo(origine);

	return s;
}


/*-------------------------------Serveur UDP -----------------------------------------------*/
int boucleServeurUDP(int s,int (*traitement)(unsigned char *,int,char *)){
	while(1){
		int statut;
		char * hote;
		char * service;
		hote=malloc(MAX_TAILLE_TAMPON);
		service=malloc(MAX_TAILLE_TAMPON);
		struct sockaddr_storage adresse;
		socklen_t taille=sizeof(adresse);
		unsigned char message[MAX_UDP_MESSAGE];
		int nboctets=recvfrom(s,message,MAX_UDP_MESSAGE,0,(struct sockaddr *)&adresse,&taille);
		
		if(nboctets<0) return -1;
		statut=getnameinfo((struct sockaddr *)&adresse,taille,hote,MAX_TAILLE_TAMPON,service,MAX_TAILLE_TAMPON,NI_NUMERICHOST|NI_NUMERICSERV);

		if(statut<0) {perror("Erreur fonction getnameinfo"); break;}
		if(traitement(message,nboctets,hote)<0) break;

	}
	return 0;
}
