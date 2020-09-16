#ifndef libudp
#define libudp

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_UDP_MESSAGE 1000
#define MAX_TAILLE_TAMPON 100
#define NOMBRE_DE_BOT 5
#define TAILLE_IP_BOT 15
#define TAILLE_ID_BOT 6

#endif

/* prototypes */
int createSocket(char *hote,char *service, void** handle_res);
void messageUDP(int s,struct addrinfo * handle_res,unsigned char *message,int taille);
int boucleServeurUDP(int s,int (*traitement)(unsigned char *,int,char *));
int initialisationSocketUDP(char *service);
