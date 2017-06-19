#include "../headers/child.h"

int child(int child_id, shm_t **shm_array, int pipe_fd, int queue_id, lock_t *sem_ids) {
	cmd_t cmd;
	msg_t msg;
	while (true) {
		if (rcv_cmd(&cmd, pipe_fd, child_id, sem_ids->pipe_sem) == -1)
			sys_err("ERROR rcv_cmd figlio\n");
		else {
			msg.type = 1;
			msg.cmd = cmd;
			msg.id = child_id;
			switch (cmd.role) {
				case MULTIPLY:
					#ifdef DEBUG
					sys_print(STDOUT, "FIGLIO %i\tMULTIPLY\ti: %i, j: %i\n", child_id, cmd.data.c.i, cmd.data.c.j);
					#endif
					multiply(cmd.data.c.i, cmd.data.c.j, shm_array);
				break;
				case SUM:
					#ifdef DEBUG
					sys_print(STDOUT, "FIGLIO %i\tSUM\t\trow: %i\n", child_id, cmd.data.row);
					#endif
					if (sum(cmd.data.row, shm_array, sem_ids)) {
						msg.success = false;
						if(send_msg(&msg, queue_id, sem_ids->queue_sem) == -1) {
							sys_err("ERROR send_msg figlio");
							return -1;
						}
					}
				break;
				case END:
					#ifdef DEBUG
					sys_print(STDOUT, "FIGLIO %i\tEND\n", getpid());
					#endif
					return 0;
			}
			msg.success = true;

			if(send_msg(&msg, queue_id, sem_ids->queue_sem) == -1) {
				sys_err("send_msg child");
				return -1;
			}
		}
	}
	return 0;
}


void multiply(int i, int j, shm_t **shm_array) {
	shm_t *A = shm_array[0];
	shm_t *B = shm_array[1];
	shm_t *C = shm_array[2];

	long *row = (long *) malloc(sizeof(long) * A->N);
	long *col = (long *) malloc(sizeof(long) * B->N);
	long res = 0;

	for (int l = 0; l < A->N; l++) {
		row[l] = A->shmaddr[i * A->N + l];
		col[l] = B->shmaddr[j + l * B->N];
	}

	for (int l = 0; l < A->N; l++) {
		res += row[l] * col[l];
	}

	C->shmaddr[i * C->N + j] = res;

	free(row);
	free(col);
}


int sum(int k, shm_t **shm_array, lock_t *sem_ids) {
	shm_t *C = shm_array[2];
	shm_t *S = shm_array[3];

	long res = 0;

	for (int i = 0; i < C->N; i++) {
		res += C->shmaddr[k * C->N + i];
	}

	if (sem_lock(sem_ids->S_sem, 0) == -1) {
		sys_err("ERROR sum - sem_lock S");
		return -1;
	}
	S->shmaddr[0] += res;
	sem_unlock(sem_ids->S_sem, 0);

	return 0;
}
