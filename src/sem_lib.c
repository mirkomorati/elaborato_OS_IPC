#include "../headers/std_lib.h"
#include "../headers/sem_lib.h"
#include "../headers/ending_lib.h"


int sem_create(int nsem, int init) {
	int id;
	sig_sem_t tmp_sem;
	unsigned short semctl_arg[nsem];

	for (int i = 0; i < nsem; ++i)
		semctl_arg[i] = init;

	if ((id = semget(IPC_PRIVATE, nsem, IPC_CREAT | 0666)) == -1){
		sys_err("semaphore create");
		return -1;
	}

	if (semctl(id, 0, SETALL, semctl_arg) == -1){
		sys_err("semaphore init");
		return -1;
	}

	tmp_sem.semid = id;
	tmp_sem.semnum = 1;

	sig_add_sem(1, &tmp_sem);

	return id;
}


int sem_lock(int id, int nsem) {
	struct sembuf sem_op;

	sem_op.sem_num = nsem;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	if (semop(id, &sem_op, 1) == -1) {
		sys_err("ERROR semaphore lock");
		return -1;
	}
	return 0;
}


int sem_unlock(int id, int nsem) {
	struct sembuf sem_op;

	sem_op.sem_num = nsem;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	if (semop(id, &sem_op, 1) == -1) {
		sys_err("ERROR semaphore unlock");
		return -1;
	}
	return 0;
}


int sem_dec(int id, int nsem) {
	return sem_lock(id, nsem);
}


int sem_inc(int id, int nsem) {
	return sem_unlock(id, nsem);
} 