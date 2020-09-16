#include "libipc.h"

/* construit une file de message IPC */
int construit_file_msg_ipc(key_t clee_ipc){
	int msgid;
	if ((msgid = msgget(clee_ipc, IPC_CREAT|IPC_EXCL|0666)) == 1) {
		 perror ("Erreur lors de la construction de la file de message IPC");
		 return -1;
	 }
	 return 0;
 }

/* envoie une message sur la file de message IPC */
void envoie_msg_ipc(key_t clee_ipc, Msg_IPC_t message){
	int msgid;
	if ((msgid = msgget(clee_ipc,0)) == 1) {
		perror ("msgget");
		exit(1);
	}

	msgsnd(msgid, &message, TAILLE_MSG, 0);
}

/* reçoit un message de type "type", de file "clee_ipc", et modifie la strucutre pointée "pt_message" en conséquence*/
int recoit_msg_ipc(key_t clee_ipc, int type, Msg_IPC_t* pt_message){
	int msgid;

	if ((msgid = msgget(clee_ipc,0)) == 1) {
		perror ("msgget");
		exit(1);
 	}

	/* attente message de type "type"
	   si on le trouve, on le supprime de msgid et on le met dans "message"
	   si non on revient immédiatement grâce à IPC_NOWAIT */
	if(msgrcv(msgid, pt_message, TAILLE_MSG, type, IPC_NOWAIT) > 0){
		return 0;
	}
	else
		return -1;
}

/* détruit la file de message IPC*/
void detruit_file_msg(key_t clee_ipc){
	int msgid;
	if ((msgid = msgget(clee_ipc,0)) == 1) {
		perror ("msgget");
		return;
	}
	msgctl(msgid, IPC_RMID, 0);
}

/* init mémoire partagée IPC */
void init_mem_partagee(key_t clee_mem_partagee, int taille_memoire_partagee){
	if (shmget(clee_mem_partagee, taille_memoire_partagee, 0666 | IPC_CREAT | IPC_EXCL) == -1){
		printf("init shmget");
		return ;
	}
}

/* init sémaphore */
void init_semaphore(key_t clee_semaphore, struct sembuf tab_semaphores[NB_SEMAPHORE]){
	int id_semaphore;

	/* déclare sémaphore */
	if ((id_semaphore=semget(clee_semaphore, NB_SEMAPHORE, 0666 | IPC_CREAT | IPC_EXCL )) == -1 ){
		perror("init semget");
		return ;
	}

	/* init semaphore */
	tab_semaphores[0].sem_num=0;
	tab_semaphores[0].sem_op=1;

	tab_semaphores[1].sem_num=1;
	tab_semaphores[1].sem_op=1;

	semop(id_semaphore,tab_semaphores,2);
 }

 /* detruit la mémoire partagée */
 void detruit_mem_partagee(key_t clee_mem_partagee, int taille_memoire_partagee){
	 int id_mem_partagee;

 	if ((id_mem_partagee = shmget(clee_mem_partagee, taille_memoire_partagee, 0666)) == -1){
 		perror("recup shmget");
 		return;
 	}

	/* supprime mémoire partagée */
 	if (shmctl(id_mem_partagee, IPC_RMID, NULL )==-1){
 		perror("shmctl");
 		return;
 	}
 }

 /* détruit sémaphore */
void detruit_semaphore(key_t clee_semaphore){
	int id_semaphore;

	/* recup id semaphore */
	if ((id_semaphore = semget(clee_semaphore, NB_SEMAPHORE, 0666)) == -1 ){
		//perror("recup semget");
		return;
	}
	/* supprime sémaphore */
	if (semctl(id_semaphore, 1, IPC_RMID )==-1){
		perror("semctl");
		return;
	}
}

/* écrit message sur la mémoire partagée */
void ecrit_mem_partagee(key_t clee_mem_partagee, key_t clee_semaphore, struct sembuf tab_semaphores[NB_SEMAPHORE], unsigned char* message_a_partagee, int taille_memoire_partagee){
	int id_mem_partagee, id_semaphore;
	void* adresse_memoire=NULL;

	if ((id_mem_partagee = shmget(clee_mem_partagee, taille_memoire_partagee, 0666)) == -1){
		perror("recup shmget");
		exit(0);
	}

	if ((id_semaphore = semget(clee_semaphore, NB_SEMAPHORE, 0666)) == -1 ){
		perror("recup semget");
		exit(0);
	}

	/* Operation P sur le premier semaphore */
    tab_semaphores[0].sem_num=0;
    tab_semaphores[0].sem_op=-1;
    semop(id_semaphore,tab_semaphores,1);

    if ((adresse_memoire = shmat(id_mem_partagee, NULL, 0)) == NULL){
		perror("recup shmat");
		exit(0);
	}

	memcpy(adresse_memoire,message_a_partagee,taille_memoire_partagee);

    if (shmdt(adresse_memoire) ==-1 ){
        perror("shmdt");
        exit(0);
	}

	/* Operation V sur le premier semaphore */
    tab_semaphores[0].sem_num=0;
    tab_semaphores[0].sem_op=1;
    semop(id_semaphore,tab_semaphores,1);

  /* On attend que le sémaphore 2 soit à 0 */
	tab_semaphores[0].sem_num=1;
    tab_semaphores[0].sem_op=0;
    semop(id_semaphore,tab_semaphores,1);

}

/* lit message sur la mémoire partagée */
int lit_mem_partagee(key_t clee_mem_partagee, key_t clee_semaphore, struct sembuf tab_semaphores[NB_SEMAPHORE], unsigned char* msg_recu, int taille_memoire_partagee){
	int id_mem_partagee, id_semaphore;
	void* adresse_memoire=NULL;

	if ((id_mem_partagee = shmget(clee_mem_partagee,taille_memoire_partagee,0666)) == -1){
		return -1;
	}

	if ((id_semaphore = semget(clee_semaphore, NB_SEMAPHORE, 0666)) == -1 ){
		return -1;
	}

	/* Operation P sur le premier semaphore */
    tab_semaphores[0].sem_num=0;
    tab_semaphores[0].sem_op=-1;
    semop(id_semaphore,tab_semaphores,1);

    if ((adresse_memoire = shmat(id_mem_partagee, NULL, 0)) == NULL){
		perror("shmat");
		exit(0);
	}

	memcpy(msg_recu,adresse_memoire,taille_memoire_partagee);

    if (shmdt(adresse_memoire) ==-1 ){
        perror("shmdt");
        exit(0);
	}

	/* Operation V sur le premier semaphore */
	tab_semaphores[0].sem_num=0;
	tab_semaphores[0].sem_op=1;
	semop(id_semaphore,tab_semaphores,1);


	/* On libère le 2eme sémaphore */
	tab_semaphores[0].sem_num=1;
	tab_semaphores[0].sem_op=-1;
	semop(id_semaphore,tab_semaphores,1);

	return 0;
}
