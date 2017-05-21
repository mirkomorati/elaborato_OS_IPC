#include "../headers/std_lib.h"
#include "../headers/msg_lib.h"
#include "../headers/sem_lib.h"

int send_cmd(const cmd_t * restrict cmd, const int fd,const int id, const int sem_id){
	if (cmd != NULL){
		sem_inc(sem_id, id);
		size_t size = sizeof(*cmd);
		if (write(fd, cmd, size) == -1){
			perror("ERROR send_cmd - writing on pipe");
			return -1;
		}
	}

	return 0;
}


int rcv_cmd(cmd_t * restrict cmd, const int fd, const int id, const int sem_id){
	static char errors = 0;
	const char max_err = 2;

	if (cmd != NULL) {
		size_t size = sizeof(*cmd);
		sem_dec(sem_id, id);
		printf("superato il semaforo\n");
		if (read(fd, cmd, size) == -1) {
			perror("ERROR rcv_cmd - reading pipe");
			return -1;
		}	
	}
	return 0;
}

/*
int rcv_cmd(cmd_t * restrict cmd, const int fd, const int id, const int sem_id){
	static char errors = 0;
	const char max_err = 2;
	// impostazioni per read con timeout della pipe per rendere possibile 
	// il ctrl-c.
	fd_set set;
	struct timeval timeout;

	FD_ZERO(&set);
	FD_SET(fd, &set);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	if (cmd != NULL){
		size_t size = sizeof(*cmd);
		int tv;
		if((tv = select(fd + 1, &set, NULL, NULL, &timeout)) < 0){
			perror("timeout error");
			exit(-1);
		}
		else if (tv > 0 && read(fd, cmd, size) == -1){
			perror("ERROR rcv_cmd - reading pipe");
			return -1;
		} else if (tv > 0){
			errors = 0;
		}else{
			if (++errors > max_err){
				exit(-1);
			}
		}
	}
	return 0;
}
*/

int send_msg(const msg_t * restrict msg, const int id, const int sem_id){

	if (msg != NULL){
		if(sem_lock(sem_id, 0) != -1){
			size_t size = sizeof(msg_t) - sizeof(long);
			if(msgsnd(id, msg, size, 0) == -1){
				perror("ERROR send_msg - msgsnd");
				for(int i = 0; sem_unlock(sem_id, 0) == -1 && i < 3; i++)printf("non unlocko\n");
				return -1;
			}
			for(int i = 0; sem_unlock(sem_id, 0) == -1 && i < 3; i++)printf("non unlocko\n");
		} else return -1;
	}
	return 0;
}

int rcv_msg(msg_t * restrict msg, const int id){
	if (msg != NULL){
		size_t size = sizeof(msg_t) - sizeof(long);
		if (msgrcv(id, msg, size, 1, 0) == -1) {
			// N.B. leggo sempre il tipo 1 perché non penso ci 
			// sarà bisogno di più tipi di messaggio.
			perror("ERROR rcv_msg - msgrcv");
			return -1;
		}
	}

	return 0;
}

bool cmd_equals (const cmd_t *a,const cmd_t *b){
	if(a->role != b->role) return false;
	else if(a->role == MULTIPLY){
		if (a->data.c.i != b->data.c.i || a->data.c.j != b->data.c.j) return false;
	}
	else if (a->data.row != b->data.row) return false;
	return true;
}