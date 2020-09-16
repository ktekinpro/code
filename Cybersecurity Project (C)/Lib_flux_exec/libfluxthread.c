#include "libfluxthread.h"

/* fonction intermédiaire lancée dans un thread qui nous permet de garantir la vie de la variable du thread */
void* fonction_thread_intermediaire(void* arg){
	struct_flux_t my_struct = (*(struct_flux_t*)arg);

	/* lancement de la tâche */
	my_struct.fonction(my_struct.pt_strucutre);

	/* free le malloc de struct_flux_t* structure_intermediaire après le retour de la fonction*/
	if(my_struct.pt_strucutre != NULL) free(my_struct.pt_strucutre);

	free(arg);
	return NULL;
}

int lancer_flux(void* tache(void *), void* pt_structure_tache, int taille_structure_tache){
	pthread_t tid;

	/* déclaration structure encapsulante */
	struct_flux_t* structure_intermediaire = malloc(sizeof(*structure_intermediaire));

	/* initialisation structure encapsulante */
	structure_intermediaire->fonction = tache;

	if(pt_structure_tache !=NULL){
		structure_intermediaire->pt_strucutre = malloc(taille_structure_tache);
		memcpy(structure_intermediaire->pt_strucutre,pt_structure_tache,taille_structure_tache);
	}
	else{
		structure_intermediaire->pt_strucutre = NULL;
	}

	/* thread fonction intermédiaire en mode detach */
	if(pthread_create(&tid, NULL, fonction_thread_intermediaire, (void*) structure_intermediaire) !=0){
		perror("pthread_create");
		exit(0);
	}
	if(pthread_detach(tid) !=0){
		perror("pthread_detach");
		exit(0);
	}
	return 0;
}

void synchro_thread(){
	pthread_exit(NULL);
}

void init_semaphore_thread(pthread_mutex_t* pt_mutex){
	pthread_mutex_init(pt_mutex,NULL);
}

void P_semaphore_thread(pthread_mutex_t* pt_mutex){
	pthread_mutex_lock(pt_mutex);
}

void V_semaphore_thread(pthread_mutex_t* pt_mutex){
	pthread_mutex_unlock(pt_mutex);
}

void init_signal_thread(pthread_cond_t* pt_cnd){
	pthread_cond_init(pt_cnd,NULL);
}

void attend_signal_thread(pthread_cond_t* pt_cnd, pthread_mutex_t* pt_mutex){
	pthread_cond_wait(pt_cnd,pt_mutex);
}

void envoie_signal_thread(pthread_cond_t* pt_cnd){
	pthread_cond_signal(pt_cnd);
}
