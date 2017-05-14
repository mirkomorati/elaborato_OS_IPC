#include "../headers/std_lib.h"
#include "../headers/sem_lib.h"
#include "../headers/ending_lib.h"

int sem_create() {
	int id;
	sig_sem_t tmp_sem;

	if ((id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600)) == -1){
		perror("semaphore create: ");
		sig_end(-1);
	}

	if (semctl(id, 0, SETVAL, 1) == -1){
		perror("semaphore init: ");
		sig_end(-1);
	}

	tmp_sem.semid = id;
	tmp_sem.semnum = 1;

	sig_add_sem(1, &tmp_sem);

	return id;
}

void sem_lock(int id) {
	struct sembuf sem_op;

	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	if (semop(id, &sem_op, 1) == -1) {
		perror("semaphore lock: ");
		sig_end(-3);
	}
}

void sem_unlock(int id) {
	struct sembuf sem_op;

	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	if (semop(id, &sem_op, 1) == -1) {
		perror("semaphore unlock: ");
		sig_end(-3);
	}
}