#include "../headers/std_lib.h"
#include "../headers/ending_lib.h"


static inline void free_list_shm(sig_shmem_list_t *list){
	if(list->next != NULL) free_list_shm(list->next);

	free(list);
}


static inline void free_list_sem(sig_sem_list_t *list){
	if(list->next != NULL) free_list_sem(list->next);

	free(list);
}


static inline void free_list_queue(sig_queue_list_t *list){
	if(list->next != NULL) free_list_queue(list->next);

	free(list);
}


void sig_add_shmem(int n, ...){
	static sig_shmem_list_t *list = NULL;
	sig_shmem_list_t *cur;
	int i = 0;

	if (n <= 0) return;

	va_list ap;
	va_start(ap, n);
	if (list == NULL) {
		list = (sig_shmem_list_t *) malloc(sizeof(sig_shmem_list_t));
		list->next = NULL;
		
		sig_shmem_t *actual_obj = va_arg(ap, sig_shmem_t *);

		list->obj.shmid = actual_obj->shmid;
		list->obj.shmaddr = actual_obj->shmaddr;

		sig_init(list, NULL, NULL);
		i++;
	}

	cur = list;

	while(cur->next != NULL)
		cur = cur->next;

	for (; i < n; ++i){
		cur->next = (sig_shmem_list_t *) malloc(sizeof(sig_shmem_list_t));
		cur = cur->next;

		sig_shmem_t *actual_obj = va_arg(ap, sig_shmem_t *);

		cur->obj.shmid = actual_obj->shmid;
		cur->obj.shmaddr = actual_obj->shmaddr;

		if(n-i == 1) cur->next = NULL;
	}
	va_end(ap);

    //debug_print_shm_list(list);
}


void sig_add_sem(int n, ...){
	static sig_sem_list_t *list = NULL;
	sig_sem_list_t *cur;
	int i = 0;

	if (n <= 0) return;

	va_list ap;
	va_start(ap, n);
	if (list == NULL) {
		list = (sig_sem_list_t *) malloc(sizeof(sig_sem_list_t));
		list->next = NULL;
		
		sig_sem_t *actual_obj = va_arg(ap, sig_sem_t *);

		list->obj.semid = actual_obj->semid;
		list->obj.semnum = actual_obj->semnum;

		sig_init(NULL, list, NULL); 
		i++;
	}

	cur = list;

	while(cur->next != NULL)
		cur = cur->next;

	for (; i < n; ++i){
		cur->next = (sig_sem_list_t *) malloc(sizeof(sig_sem_list_t));
		cur = cur->next;

		sig_sem_t *actual_obj = va_arg(ap, sig_sem_t *);

		cur->obj.semid = actual_obj->semid;
		cur->obj.semnum = actual_obj->semnum;

		if(n-i == 1) cur->next = NULL;
	}
	va_end(ap);

	//debug_print_list(list);
}


void sig_add_queue(int n, ...){
	static sig_queue_list_t *list = NULL;
	sig_queue_list_t *cur;
	int i = 0;

	if (n <= 0) return;

	va_list ap;
	va_start(ap, n);
	if (list == NULL) {
		list = (sig_queue_list_t *) malloc(sizeof(sig_queue_list_t));
		list->next = NULL;
		
		list->obj = va_arg(ap, int);

		sig_init(NULL, NULL, list);
		i++;
	}

	cur = list;

	while(cur->next != NULL)
		cur = cur->next;

	for (; i < n; ++i){
		cur->next = (sig_queue_list_t *) malloc(sizeof(sig_queue_list_t));
		cur = cur->next;

		cur->obj = va_arg(ap, int);

		if(n-i == 1) cur->next = NULL;
	}
	va_end(ap);
}


void sig_handler(int sig, int pid){
	static int allowed_pid = 0;

	if (sig != -1 && allowed_pid == getpid()){
    	int status, wpid;
    	while((wpid = wait(&status)) > 0)
    		sys_print(STDOUT, "il figlio %i ha terminato\n", wpid);

    	sig_free_sem(false, NULL);
    	sig_free_memory(false, NULL);
    	sig_free_queue(false, NULL);
    	exit(sig);
    }else if (sig != -1){
    	// siamo nella terminazione del figlio.
    	sig_shmdt(false, NULL);
    }
    else {
    	allowed_pid = pid;
    }
}


void sig_init(sig_shmem_list_t *shm_list, sig_sem_list_t *sem_list, sig_queue_list_t *queue_list){

    signal(SIGINT, (void (*)(int))sig_handler);
    signal(SIGHUP, (void (*)(int))sig_handler);
    signal(SIGKILL, (void (*)(int))sig_handler);
    signal(SIGTERM, (void (*)(int))sig_handler);
    signal(SIGSEGV, (void (*)(int))sig_handler);
    signal(SIGABRT, (void (*)(int))sig_handler);

    sig_handler(-1, getpid());

    if (shm_list != NULL){
    	sig_shmdt(true, shm_list); 
    	sig_free_memory(true, shm_list); 
    }
    if (sem_list != NULL) sig_free_sem(true, sem_list);
    if (queue_list != NULL) sig_free_queue(true, queue_list);
}


void sig_free_memory(bool setting, sig_shmem_list_t *arg){
	static sig_shmem_list_t *list = NULL;

	if(setting) list = arg;
	else if(list != NULL) {
		sig_shmdt(false, NULL);
		sig_shmem_list_t *head = list;
        while(list != NULL){
            if (shmctl(list->obj.shmid, IPC_RMID, NULL) == -1){
                sys_print(STDOUT, "ERROR: shmid: %i", list->obj.shmid);
                sys_err("shmctl sig_free_memory");
                return;
            }
            list = list->next;
        }
       
        free_list_shm(head);
	}
}


void sig_shmdt(bool setting, sig_shmem_list_t *arg){
	static sig_shmem_list_t *list = NULL;

	if(setting) list = arg;
	else if(list != NULL) {
        while(list != NULL){
            if (shmdt(list->obj.shmaddr) == -1) {
                sys_err("shmdt");
                return;
            }
            list = list->next;
        }
	}
}


void sig_free_sem(bool setting, sig_sem_list_t *arg){
	static sig_sem_list_t *list = NULL;

	if(setting) list = arg;
	else if(list != NULL) {
		sig_sem_list_t *head = list;
        while(list != NULL){
            if (semctl(list->obj.semid, list->obj.semnum, IPC_RMID) == -1){
                sys_err("semctl sig_free_sem");
                return;
            }
            list = list->next;
        }
       
        free_list_sem(head);
	}
}


void sig_free_queue(bool setting, sig_queue_list_t *arg){
	static sig_queue_list_t *list = NULL;

	if(setting) list = arg;
	else if(list != NULL) {
		sig_queue_list_t *head = list;
        while(list != NULL){
            if (msgctl(list->obj, IPC_RMID, NULL) == -1){
                sys_err("msgctl");
                return;
            }
            list = list->next;
        }
       
        free_list_queue(head);
	}
}


void sig_end(int code){
	int status, wpid;
    while((wpid = wait(&status)) > 0)
    	sys_print(STDOUT, "terminazione normale: il figlio %i ha terminato\n", wpid);
	// stessa cosa di sig_handler
	sig_free_sem(false, NULL);
	sig_free_memory(false, NULL);
	sig_free_queue(false, NULL);
	exit(code);
}