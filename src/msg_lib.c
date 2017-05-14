#include "../headers/std_lib.h"
#include "../headers/msg_lib.h"
#include "../headers/sem_lib.h"

int send_cmd(const cmd_t * restrict cmd, const int * restrict id){
	static int pipe_fd = -1; // non esistono fd negativi.
	
	if (id != NULL) pipe_fd = *id;

	if (cmd != NULL){
		size_t size = sizeof(*cmd);
		if (write(pipe_fd, cmd, size) == -1){
			perror("writing on pipe: ");
			return -1;
		}
	}

	return 0;
}

int rcv_cmd(cmd_t * restrict cmd, const int * restrict id){
	static int pipe_fd = -1;
	static int sem_id = -1;

	if (id != NULL) pipe_fd = *id;

	if (sem_id == -1) sem_id = sem_create();

	if (cmd != NULL){
		sem_lock(sem_id);
		size_t size = sizeof(*cmd);
		if (read(pipe_fd, cmd, size) == -1){
			perror("reading pipe: ");
			sem_unlock(sem_id);
			return -1;
		}
		sem_unlock(sem_id);
	}
	return 0;
}

int send_msg(const msg_t * restrict msg, const int * restrict id){
	static int msg_queue_id = -1;
	static int sem_id = -1;

	if (id != NULL) msg_queue_id = *id;

	if (sem_id == -1) sem_id = sem_create();

	if (msg != NULL){
		sem_lock(sem_id);
		size_t size = sizeof(msg_t) - sizeof(long);
		if(msgsnd(msg_queue_id, msg, size, 0) == -1){
			perror("sending msg: ");
			sem_unlock(sem_id);
			return -1;
		}
		sem_unlock(sem_id);
	}
	return 0;
}

int rcv_msg(msg_t * restrict msg, const int * restrict id){
	static int msg_queue_id = -1; // non esistono fd negativi.
	
	if (id != NULL) msg_queue_id = *id;

	if (msg != NULL){
		size_t size = sizeof(msg_t) - sizeof(long);
		if (msgrcv(msg_queue_id, msg, size, 0, 0)){
			// N.B. leggo sempre il tipo 0 perché non penso ci 
			// sarà bisogno di più tipi di messaggio.
			perror("reading from queue: ");
			return -1;
		}
	}

	return 0;
}