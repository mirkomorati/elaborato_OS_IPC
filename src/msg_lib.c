#include "../headers/std_lib.h"
#include "../headers/msg_lib.h"
#include "../headers/sem_lib.h"

int send_cmd(const cmd_t * restrict cmd, const int fd){
	if (cmd != NULL){
		size_t size = sizeof(*cmd);
		if (write(fd, cmd, size) == -1){
			perror("writing on pipe: ");
			return -1;
		}
	}

	return 0;
}

int rcv_cmd(cmd_t * restrict cmd, const int fd, const int sem_id){

	if (cmd != NULL){
		sem_lock(sem_id);
		size_t size = sizeof(*cmd);
		if (read(fd, cmd, size) == -1){
			perror("reading pipe: ");
			sem_unlock(sem_id);
			return -1;
		}
		sem_unlock(sem_id);
	}
	return 0;
}

int send_msg(const msg_t * restrict msg, const int id, const int sem_id){

	if (msg != NULL){
		sem_lock(sem_id);
		size_t size = sizeof(msg_t) - sizeof(long);
		if(msgsnd(id, msg, size, 0) == -1){
			perror("sending msg: ");
			sem_unlock(sem_id);
			return -1;
		}
		sem_unlock(sem_id);
	}
	return 0;
}

int rcv_msg(msg_t * restrict msg, const int id){
	if (msg != NULL){
		size_t size = sizeof(msg_t) - sizeof(long);
		if (msgrcv(id, msg, size, 0, 0)){
			// N.B. leggo sempre il tipo 0 perché non penso ci 
			// sarà bisogno di più tipi di messaggio.
			perror("reading from queue: ");
			return -1;
		}
	}

	return 0;
}