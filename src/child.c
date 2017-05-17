#include "../headers/child.h"

int child(shm_t **shm_array, int pipe_fd, int queue_id, int *sem_id_array){
	cmd_t cmd;
	while(true){
		if(rcv_cmd(&cmd, pipe_fd, sem_id_array[0]) == -1)
			printf("non dovrebbe accadere CMD\n");
		if(cmd.role == END) return 0;
				
		msg_t msg;
		msg.type = 1;
		msg.success = true;
		msg.cmd = cmd;

		if(send_msg(&msg, queue_id, sem_id_array[1]) == -1) {
			perror("send_msg child");
			return -1;
		}
	}
}