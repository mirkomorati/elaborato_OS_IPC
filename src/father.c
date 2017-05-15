#include "../headers/father.h"

int init(shm_t **shm_array) {
#ifdef DEBUG   
    printf("Loading matrix\n");
#endif

    bool parse;
    for (int i = 0; shm_array[i] != NULL; i++) {
        #ifdef DEBUG
        printf("INIT => %s\n", shm_array[i]->path);
        #endif
        parse = true;
        if (shm_array[i + 2] == NULL || shm_array[i + 1] == NULL)
            parse = false;

        if (shm_load(shm_array[i], parse) == -1) {
            perror("shm_load");
            return -1;
        }
    }
/*
    if (shmatrix_load(&A, N) == -1) {
        perror("shmatrix_load A");
        exit(1);
    }

    {
        sig_shmem_t tmp;
        tmp.shmid = A.shmid;
        tmp.shmaddr = A.shmaddr;
        sig_add_shmem(1, &tmp);
    }

    if (shmatrix_load(&B, N) == -1) {
        perror("shmatrix_load B");
        exit(1);
    }
    
    {
        sig_shmem_t tmp;
        tmp.shmid = B.shmid;
        tmp.shmaddr = B.shmaddr;
        sig_add_shmem(1, &tmp);
    }

    if (shm_create(&C, N) == -1) {
        perror("shm_create C");
        exit(1);
    }

    {
        sig_shmem_t tmp;
        tmp.shmid = C.shmid;
        tmp.shmaddr = C.shmaddr;
        sig_add_shmem(1, &tmp);
    }

#ifdef DEBUG
    printf("Creating shm for sum\n");
#endif
    S.path = "/dev/urandom";
    if (shm_create(&S, 1) == -1) {
        perror("shsum_create S");
        exit(1);
    }

    {
        sig_shmem_t tmp;
        tmp.shmid = S.shmid;
        tmp.shmaddr = S.shmaddr;
        sig_add_shmem(1, &tmp);
    }

#ifdef DEBUG
    int sem_id = sem_create();
    sem_lock(sem_id);
    sem_unlock(sem_id);
#endif

#ifdef DEBUG
    printf("attendo ctrl-c..\n");
    for (int i = 0; i < 10; i++){
      usleep(1e6);
    }
#endif
*/
    return 0;
}

int make_child(shm_t **shm_array , int P, int *pipe_fd, int *queue_id) {
    return 0;
}

int run(int P, int pipe, int queue) {
    return 0;
}

int main(int argc, char **argv) {
    /*! 
     * \struct Struttura da passare a getopt() per l'alias delle opzioni
     */
    static struct option long_opt[] = {
        {"matrixA",   required_argument, 0, 'A'},
        {"matrixB",   required_argument, 0, 'B'},
        {"matrixC",   required_argument, 0, 'C'},
        {"order",     required_argument, 0, 'N'},
        {"processes", required_argument, 0, 'P'},
        {"help",      no_argument,       0, 'h'},
        {0,           0,                 0,  0 }
    };

    int opt;
    char *short_opt = "A:B:C:N:P:h";

    shm_t A, B, C;
    shm_t S;
    int N;
    int P;

    if (argc < 10) {
        char *buf = "Error: too few arguments.\nusage: ./elaborato_IPC -A matrix -B matrix -C matrix -N order -P #processes\n";
        write(STDOUT, buf, sizeof(char) * strlen(buf));
        exit(1);
    }

    while ((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        if (opt == -1) break;
        switch (opt) {
            case 'A': 
                A.path = optarg;
                break;

            case 'B':
                B.path = optarg;
                break;

            case 'C':{
                // controllo se esiste il file altrimenti lo creo.
                struct stat st;
                if(stat(optarg, &st) == -1 && errno == ENOENT)
                    if(creat(optarg, S_IRUSR) == -1)
                        perror("Error creating matrixC file: ");
                C.path = optarg;
                break;
            }
            case 'N':
                N = atoi(optarg);
                break;
            
            case 'P':
                P = atoi(optarg);
                break;

            case 'h': {
                char *buf = "usage: ./elaborato_IPC -A matrix -B matrix -C matrix -N order -P #processes\n";
                write(STDOUT, buf, sizeof(char) * strlen(buf));
                break;
            }
            default:
                printf("Wrong arguments\n");
                printf("Usage: \n");
                exit(1);
        }
    }
    A.N = N;
    B.N = N;
    C.N = N;
    S.N = 1;

    shm_t *shm_array[5] = {&A, &B, &C, &S, NULL};
    int pipe_fd, queue_id;

    if(init(shm_array) == -1) {
        perror("init");
        sig_end(-1);
    }

    if (make_child(shm_array, P, &pipe_fd, &queue_id) == -1){
        perror("make_child");
        sig_end(-1);
    }

    sig_end(run(P, pipe_fd, queue_id));
}
