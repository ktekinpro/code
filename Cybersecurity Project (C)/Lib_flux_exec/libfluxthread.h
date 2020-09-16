#ifndef libfluxthread
#define libfluxthread

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#endif

/* structure lib flux pour lancer la fonction intermédiaire dans le thread */
typedef struct{
	void* (*fonction)(void* );
	void* pt_strucutre;
} struct_flux_t;

/* typedef pour encapsuler les types de certaine variable */
typedef pthread_mutex_t mutex_flux_t;
typedef pthread_cond_t signal_flux_t;

/* prototypes */
void* fonction_thread_intermediaire(void* arg);
int lancer_flux(void* tache(void *), void* pt_structure_tache, int taille_structure_tache);
void synchro_thread();
void init_semaphore_thread(pthread_mutex_t* mutex);
void P_semaphore_thread(pthread_mutex_t* mutex);
void V_semaphore_thread(pthread_mutex_t* mutex);
void init_signal_thread(pthread_cond_t* pt_cnd);
void attend_signal_thread(pthread_cond_t* pt_cnd, pthread_mutex_t* pt_mutex);
void envoie_signal_thread(pthread_cond_t* pt_cnd);
