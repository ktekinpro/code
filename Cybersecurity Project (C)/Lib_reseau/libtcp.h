#ifndef libtcp
#define libtcp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <time.h>

#define _GNU_SOURCE
#define MAX_LIGNE 100
#define TCP_NODELAY 5
#define MAX_CONNEXIONS 100
#define MAX_TAMPON 20
#define TAILLE_IP_BOT 15
#define NB_MAX_EXECUTION 50
#define NB_MAX_CHARGES 50
#define NB_MAX_EXECUTION 50

#endif

/* structure pour l'upload */
typedef struct {
  char * name;
  unsigned char * data;
  int name_size;
  int data_size;
  int status;
} charge_t;

/* structure pour recevoir et envoyer une charge */
typedef struct{
    charge_t ma_charge;
    char* mon_ip;
    int ip_size;
} charge_shm_t;

/* strucutre ip */
typedef struct{
  char ip_du_bot[TAILLE_IP_BOT];
  char* status;
  int id_unique;
} struc_bot_t;

/* structure pour stocker les resultat d'exécution d'une charge */
typedef struct{
    char* file_resultat_name;
    char* charge_name;
    int data_size;
    int index;
} EXEC_STRUCT_t;

/* tableau des noms de charge */
typedef struct {
    char *tab_nom[NB_MAX_CHARGES];
    int indice;
    pthread_mutex_t mutex_nom_charge;
} tab_nom_charge_t;

/* prototypes */
int initialisationServeur(char *service,int connexions);
int boucleServeur(int ecoute,int (*traitement)(int));
int gestionClient(int s);
int connexionServeur(char *hote,char *service);
