#include "../headers/father.h"

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
    S.path = "/dev/urandom";

    shm_t *shm_array[5] = {&A, &B, &C, &S, NULL};
    int sem_id_array[7] = {0, 0, 0, 0, 0, 0, -1};

    int pipe_fd, queue_id;

    if(init(shm_array, sem_id_array) == -1) {
        perror("init");
        sig_end(-1);
    }

    if (make_child(shm_array, sem_id_array, P, &pipe_fd, &queue_id) == -1){
        perror("make_child");
        sig_end(-1);
    }

    sig_end(run(N, P, pipe_fd, queue_id));
}

int init(shm_t **shm_array, int *sem_id_array) {

    #ifdef DEBUG   
    printf("Loading matrix\n\n");
    #endif

    bool parse;
    for (int i = 0; shm_array[i] != NULL; i++) {
        parse = true;
        if (shm_array[i + 2] == NULL || shm_array[i + 1] == NULL)
            parse = false;

        if (shm_load(shm_array[i], parse) == -1) {
            perror("shm_load");
            return -1;
        }
    }

    for (int i = 0; sem_id_array[i] != -1; i++) {
        sem_id_array[i] = sem_create();
    }
    return 0;
}

int make_child(shm_t **shm_array , int *sem_id_array, int P, int *pipe_fd, int *queue_id) {
    int tmp_pipe[2];
    int tmp_queue_id;
    int pids[P];
    #ifdef DEBUG
    printf("creating pipe\n");
    #endif
    if(pipe(tmp_pipe) == -1){
        perror("ERROR make_child - creating pipe");
        return -1;
    }
    *pipe_fd = tmp_pipe[1];

    #ifdef DEBUG
    printf("creating queue\n");
    #endif
    if((tmp_queue_id =msgget(IPC_PRIVATE, (IPC_CREAT | IPC_EXCL | 0666))) == -1){
        perror("ERROR make_child - creating queue");
        return -1;
    }
    sig_add_queue(1, tmp_queue_id);
    *queue_id = tmp_queue_id;
    
    #ifdef DEBUG
    printf("creating childs\n");
    #endif

    for (int i = 0; i < P; ++i){
        if((pids[i] = fork()) < 0){
            perror("ERROR make_child - creating child");
            return -1;
        }
        else if (pids[i] == 0){
            exit(child(shm_array, tmp_pipe[0], tmp_queue_id, sem_id_array));
        }
    }

    #ifdef DEBUG
    printf("\n%i childs created:\n", P);
    for (int i = 0; i < P; ++i){
        printf("child %i: pid = %i\n",i, pids[i]);
    }
    printf("\n");
    #endif
    return 0;   
}

int run(int N, int P, int pipe, int queue) {
    int completed_rows[N];
    int i = 0, j = 0, errors = 0;
    for (int p = 0; p < P; ++p) {
        cmd_t cmd;
        cmd.role = MULTIPLY;
        cmd.data.c.i = i;
        cmd.data.c.j = j;
        if (send_cmd(&cmd, pipe) == -1) {
            if(++errors > MAX_ERRORS) {
                perror("too many errors");
                return -1;
            }
        } else {
            i += ++j / N;
            j %= N;
        }
    }

    errors = 0;

    while(true){
        cmd_t cmd;
        msg_t msg;
        if (i >= N || j > N){ 
            printf("waiting for ctrl-c...\n");
            usleep(5e6);
            cmd.role = END;
            for (int p = 0; p < P; ++p)
            {
                if (send_cmd(&cmd, pipe) == -1) 
                    perror("sending end cmd");
            }
            break;
        }
        if (rcv_msg(&msg, queue) == -1) {
            if(++errors > MAX_ERRORS) {
                perror("too many errors");
                return -1;
            }
        }
        if(msg.success) {
            if(++completed_rows[msg.cmd.data.c.i] == N) {
                cmd.role = SUM;
                cmd.data.row = msg.cmd.data.c.i;   
            } else {
                i += ++j/N;
                j %= N;
                cmd.role = MULTIPLY;
                cmd.data.c.i = i;
                cmd.data.c.j = j;
            }

            if (send_cmd(&cmd, pipe) == -1) {
                if(++errors > MAX_ERRORS) {
                    perror("too many errors");
                    return -1;
                } else {
                    j--; // da tenere così.
                    j %= N;
                    i -= j / N;
                }
            }
        } else {
            #ifdef DEBUG
            printf("Message received FAIL\n");
            #endif
            while (send_cmd(&msg.cmd, pipe) == -1) {
                if(++errors > MAX_ERRORS) {
                    perror("too many errors");
                    return -1;
                } 
            }
        }
    }
    return 0;
}