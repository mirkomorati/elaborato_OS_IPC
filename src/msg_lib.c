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
	if (cmd != NULL) {
		size_t size = sizeof(*cmd);
		sem_dec(sem_id, id);
		if (read(fd, cmd, size) == -1) {
			perror("ERROR rcv_cmd - reading pipe");
			return -1;
		}	
	}
	return 0;
}


int send_msg(const msg_t * restrict msg, const int id, const int sem_id){

	if (msg != NULL){
		if(sem_lock(sem_id, 0) != -1){
			size_t size = sizeof(msg_t) - sizeof(long);
			if(msgsnd(id, msg, size, 0) == -1){
				perror("ERROR send_msg - msgsnd");
				for(int i = 0; sem_unlock(sem_id, 0) == -1 && i < 3; i++)
					sys_print(STDOUT, "non unlocko\n");
				return -1;
			}
			for(int i = 0; sem_unlock(sem_id, 0) == -1 && i < 3; i++)
				sys_print(STDOUT, "non unlocko\n");
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