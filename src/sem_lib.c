#include "../headers/std_lib.h"
#include "../headers/sem_lib.h"
#include "../headers/ending_lib.h"

int sem_create() {
	int id;
	sig_sem_t tmp_sem;

	if ((id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) == -1){
		perror("semaphore create");
		return -1;
	}

	if (semctl(id, 0, SETVAL, 1) == -1){
		perror("semaphore init");
		return -1;
	}

	tmp_sem.semid = id;
	tmp_sem.semnum = 1;

	sig_add_sem(1, &tmp_sem);

	return id;
}

void sem_lock(int id) {
	struct sembuf sem_op;

	#ifdef DEBUG
	printf("Semaforo %i\tPid %i prova lock\n", id, getpid());
	#endif
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = IPC_NOWAIT;
	if (semop(id, &sem_op, 1) == -1) {
		perror("ERROR semaphore lock");
	}
}

void sem_unlock(int id) {
	struct sembuf sem_op;

	#ifdef DEBUG
	printf("Semaforo %i\tPid %i prova unlock\n", id, getpid());
	#endif
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = IPC_NOWAIT;
	if (semop(id, &sem_op, 1) == -1) {
		perror("ERROR semaphore unlock");
	}
}