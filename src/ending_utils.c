#include "../headers/ending_utils.h"

#include <stdarg.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static inline void free_list(sig_shmem_list_t *list){
	if(list->next != NULL) free_list(list->next);

	free(list);
}

void sig_add(int n, ...){
	static sig_shmem_list_t *list = NULL;
	sig_shmem_list_t *cur;
	int i = 0;

	if (n <= 0) return;

	va_list ap;
	va_start(ap, n);
	if (list == NULL) {
		list = (sig_shmem_list_t *) malloc(sizeof(sig_shmem_list_t));
		list->next = NULL;
		
		sig_utils_t *actual_obj = va_arg(ap, sig_utils_t *);

		list->obj.shmid = actual_obj->shmid;
		list->obj.shmaddr = actual_obj->shmaddr;

		sig_init(list);
		i++;
	}

	cur = list;

	while(cur->next != NULL)
		cur = cur->next;

	for (; i < n; ++i){
		cur->next = (sig_shmem_list_t *) malloc(sizeof(sig_shmem_list_t));
		cur = cur->next;

		sig_utils_t *actual_obj = va_arg(ap, sig_utils_t *);

		cur->obj.shmid = actual_obj->shmid;
		cur->obj.shmaddr = actual_obj->shmaddr;

		if(n-i == 1) cur->next = NULL;
	}
	va_end(ap);
}

void sig_handler(int sig, void *arg){
	static sig_shmem_list_t *list = NULL;

	if (list == NULL) list = arg;

	if (sig != -1) // se non sono in fase di inizializzazione
    {
    	sig_shmem_list_t *head = list;
        while(list != NULL){
            if (shmdt(list->obj.shmaddr) == -1) {
                perror("shmdt");
                exit(-1);
            }
            if (shmctl(list->obj.shmid, IPC_RMID, NULL) == -1){
                perror("shmctl");
                exit(-2);
            }
            list = list->next;
        }
       
        free_list(head);
        exit(sig);
    }
}

void sig_init(sig_shmem_list_t *list){

    signal(SIGINT, (void (*)(int))sig_handler);
    signal(SIGHUP, (void (*)(int))sig_handler);
    signal(SIGKILL, (void (*)(int))sig_handler);
    signal(SIGTERM, (void (*)(int))sig_handler);
    sig_handler(-1,(void *)list);
    end(true, list);
}

void end(bool setting, sig_shmem_list_t *arg){
	static sig_shmem_list_t *list = NULL;

	if(setting) list = arg;
	else{
		sig_shmem_list_t *head = list;
        while(list != NULL){
            if (shmdt(list->obj.shmaddr) == -1) {
                perror("shmdt");
                exit(-1);
            }
            if (shmctl(list->obj.shmid, IPC_RMID, NULL) == -1){
                perror("shmctl");
                exit(-2);
            }
            list = list->next;
        }
       
        free_list(head);
        exit(0);
	}
}
