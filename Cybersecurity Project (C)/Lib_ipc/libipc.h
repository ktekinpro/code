#ifndef libipc
#define libipc

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define _GNU_SOURCE
#define NB_SEMAPHORE 2
#define TAILLE_MSG 512
#define TAILLE_MEM_PARTAGEE 512
#define NB_MAX_IP_BOT 100
#define TAILLE_IP 20

#endif

/* structure des messages IPC */
typedef struct {
    long type;
    char texte[TAILLE_MSG];
} Msg_IPC_t;

/* structure envoie des ips par message IPC */
typedef struct {
    char tab_IP[NB_MAX_IP_BOT][TAILLE_IP];
    int nb_bot;
    pthread_mutex_t mutex_tab_ip;
} struct_tableau_ip_t;


/* prototypes */
int construit_file_msg_ipc(key_t clee_ipc);
void envoie_msg_ipc(key_t clee_ipc, Msg_IPC_t message);
int recoit_msg_ipc(key_t clee_ipc, int type, Msg_IPC_t* pt_message);
void detruit_file_msg(key_t clee_ipc);
void detruit_semaphore(key_t clee_semaphore);
void init_semaphore(key_t clee_semaphore, struct sembuf tab_semaphores[NB_SEMAPHORE]);
void init_mem_partagee(key_t clee_mem_partagee, int taille_memoire_partagee);
void detruit_mem_partagee(key_t clee_mem_partagee, int taille_memoire_partagee);
void ecrit_mem_partagee(key_t clee_mem_partagee, key_t clee_semaphore, struct sembuf tab_semaphores[NB_SEMAPHORE], unsigned char* message_a_partagee,int taille_memoire_partagee);
int lit_mem_partagee(key_t clee_mem_partagee,key_t clee_semaphore,struct sembuf tab_semaphores[NB_SEMAPHORE],unsigned char* msg_recu,int taille_memoire_partagee);
