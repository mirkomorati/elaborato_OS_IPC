#include "../headers/child.h"

int child(shm_t **shm_array, int pipe_fd, int queue_id, int *sem_id_array){
	cmd_t cmd;
	while(true){
		if(rcv_cmd(&cmd, pipe_fd, sem_id_array[0]) == -1)
			printf("non dovrebbe accadere CMD\n");
		if(cmd.role == END) return 0;
		printf("sono il figlio %i\n\tcmd:\n\t\trole = %i\n", getpid(), cmd.role);
		if(cmd.role == MULTIPLY)
			printf("\t\ti = %i\n\t\tj = %i\n\n", cmd.data.c.i, cmd.data.c.j);
		else
			printf("\t\tk = %i\n\n", cmd.data.row);
		
		msg_t msg;
		msg.type = 1;
		msg.success = true;
		msg.cmd = cmd;

		#ifdef DEBUG
		printf("Child %i here - sending back a message\n", getpid());
		printf("\ttype: %li, success: %i\n", msg.type, msg.success);
		#endif
		if(send_msg(&msg, queue_id, sem_id_array[1]) == -1) {
			perror("send_msg child");
			return -1;
		}
	}
}