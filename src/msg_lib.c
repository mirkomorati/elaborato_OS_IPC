#include "../headers/std_lib.h"
#include "../headers/msg_lib.h"
#include "../headers/ending_lib.h"

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

