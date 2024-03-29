#include "../headers/father.h"

// dichiaro alcune funzioni statiche 

static inline int first_free(char *a, int dim);


int main(int argc, char **argv) {
    /*! 
     * \brief   struttura da passare a getopt() per l'alias delle opzioni
     * 
     */
    static struct option long_opt[] = {
        {"matrixA",   required_argument, 0, 'A'},
        {"matrixB",   required_argument, 0, 'B'},
        {"matrixC",   required_argument, 0, 'C'},
        {"order",     required_argument, 0, 'N'},
        {"processes", required_argument, 0, 'P'},
        {"thread",    no_argument,       0, 't'},
        {"help",      no_argument,       0, 'h'},
        {0,           0,                 0,  0 }
    };

    int opt;
    char *short_opt = "A:B:C:N:P:th";

    shm_t A, B, C, S;
    shm_t *shm_array[5] = {&A, &B, &C, &S, NULL};
    int N;
    int P;
    bool thread_flag = false;
    lock_t sem_ids;
    int queue_id;

    while ((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        if (opt == -1) break;
        switch (opt) {
            case 'A': 
                A.path = optarg;
                break;

            case 'B':
                B.path = optarg;
                break;

            case 'C': {
                // controllo se esiste il file altrimenti lo creo.
                struct stat st;
                if(stat(optarg, &st) == -1 && errno == ENOENT)
                    if(creat(optarg, S_IRUSR | S_IWUSR) == -1)
                        sys_err("ERROR creating matrixC file");
                C.path = optarg;
                break;
            }
            
            case 'N':
                N = atoi(optarg);
                break;
            
            case 'P':
                P = atoi(optarg);
                break;

            case 't':
                thread_flag = true;
                break;

            case 'h': {
                sys_print(STDOUT, "usage: %s -A matrixA -B matrixB -C matrixC -N order [-P #processes | -t]\n", argv[0]);
                exit(0);
                break;
            }
            
            default:
                sys_print(STDOUT, "Wrong arguments\nusage: %s -A matrix -B matrix -C matrix -N order [-P #processes | -t]\n", argv[0]);
                exit(-1);
        }
    }

    if (argc < 10) {
        sys_print(STDOUT, "Error: too few arguments.\ntype '%s -h' for usage\n", argv[0]);
        exit(-1);
    }

    if (thread_flag) {
        sys_print(STDOUT, "--- USING THREADS ---\n");
        exit(use_thread(A.path, B.path, C.path, N));
    } else if (P != 0) {
        sys_print(STDOUT, "--- USING PROCESSES ---\n");
        int pid_to_pipe[P];
        int status, wpid;

        A.N = N;
        B.N = N;
        C.N = N;
        S.N = 1;
        S.path = "/dev/urandom";

        if (init(shm_array, &sem_ids, P) == -1) {
            sys_err("ERROR init");
            sig_end(-1);
        }

        if (make_child(shm_array, &sem_ids, P, pid_to_pipe, &queue_id) == -1){
            sys_err("ERROR make_child");
            sig_end(-1);
        }

        if (run(N, P, pid_to_pipe, queue_id, &sem_ids) == -1) {
            sys_err("ERROR run");
            sig_end(-1);
        }

        while((wpid = wait(&status)) > 0)
            sys_print(STDOUT, "> Il figlio %i ha terminato\n", wpid);

        sys_print(STDOUT, "\n> Il risultato della moltiplicazione tra %s e %s è stato salvato in %s\n", A.path, B.path, C.path);

        if (N < 20) {
            for (int i = 0; i < N; i++) {
                sys_print(STDOUT, "|");
                for (int j = 0; j < N; j++) {
                    sys_print(STDOUT, "%li", C.shmaddr[i * N + j]);
                    sys_print(STDOUT, j + 1 == N ? "|" : "\t");
                }
                sys_print(STDOUT, "\n");
            }
            sys_print(STDOUT, "\n");
        }
        
        shmatrix_to_csv(&C);

        sys_print(STDOUT, "\n> La somma di tutti i suoi termini è: %li\n\n", S.shmaddr[0]);

        sig_free_sem(false, NULL);
        sig_free_memory(false, NULL);
        sig_free_queue(false, NULL);
        close(A.fd);
        close(B.fd);
        close(C.fd);
        exit(0);
    } else {
        sys_print(STDOUT, "Error: Wrong arguments.\ntype '%s -h' for usage\n", argv[0]);
        exit(-1);
    }
}


int init(shm_t **shm_array, lock_t *sem_ids, int P) {
    sys_print(STDOUT, "\n--- Loading matrix ---\n");

    if (shm_load(shm_array[0], true) == -1) {
        sys_err("ERROR shm_load A");
        return -1;
    }
    if (shm_load(shm_array[1], true) == -1) {
        sys_err("ERROR shm_load B");
        return -1;
    }
    if (shm_load(shm_array[2], false) == -1) {
        sys_err("ERROR shm_load C");
        return -1;
    }
    if (shm_load(shm_array[3], false) == -1) {
        sys_err("ERROR shm_load S");
        return -1;
    }
    // Inizializzo la somma a 0
    shm_array[3]->shmaddr[0] = 0;
    
    if ((sem_ids->queue_sem = sem_create(1, 1)) == -1) {
        sys_err("ERROR sem_create queue_sem");
        return -1;
    }
    if ((sem_ids->S_sem = sem_create(1, 1)) == -1) {
        sys_err("ERROR sem_create S_sem");
        return -1;
    }
    if ((sem_ids->pipe_sem = sem_create(P, 0)) == -1) {
        sys_err("ERROR sem_create pipe_sem");
        return -1;
    }

    return 0;
}


int make_child(shm_t **shm_array , lock_t *sem_ids, int P, int *pid_to_pipe, int *queue_id) {
    int *tmp_pipe[P];
    int tmp_queue_id;
    int pids[P];

    sys_print(STDOUT, "\n--- Creating pipes ---\n");
    
    for (int i = 0; i < P; ++i) {
        tmp_pipe[i] = (int *) malloc(2 * sizeof(int));
        if(pipe(tmp_pipe[i]) == -1) {
            sys_err("ERROR make_child - creating pipe");
            return -1;
        }
    }

    sys_print(STDOUT, "\n--- Creating queue ---\n");

    if((tmp_queue_id =msgget(IPC_PRIVATE, (IPC_CREAT | IPC_EXCL | 0666))) == -1) {
        sys_err("ERROR make_child - creating queue");
        return -1;
    }
    sig_add_queue(1, tmp_queue_id);
    *queue_id = tmp_queue_id;
    
    sys_print(STDOUT, "\n--- Creating childs ---\n");

    for (int i = 0; i < P; ++i) {
        if((pids[i] = fork()) < 0) {
            sys_err("ERROR make_child - creating child");
            return -1;
        }
        else if (pids[i] == 0) {
            exit(child(i, shm_array, tmp_pipe[i][0], tmp_queue_id, sem_ids));
        } else {
            pid_to_pipe[i] = tmp_pipe[i][1];
        }
    }

    sys_print(STDOUT, "\n> %i childs created:\n", P);
    for (int i = 0; i < P; ++i) {
        sys_print(STDOUT, "> child %i:\tpid = %i\tpipe = (r:%i,w:%i)\n",i, pids[i], tmp_pipe[i][0], tmp_pipe[i][1]);
    }
    sys_print(STDOUT, "\n");

    for (int i = 0; i < P; ++i) {
        free(tmp_pipe[i]);
    }

    return 0;   
}


int run(int N, int P, int *pid_to_pipe, int queue, lock_t *sem_ids) {
    sys_print(STDOUT, "--- Running ---\n\n");
    cmd_t *multiply_cmd_array = (cmd_t *) malloc(sizeof(cmd_t) * N * N);
    cmd_t *sum_cmd_array = (cmd_t *) malloc(sizeof(cmd_t) * N);
    int number_of_cmd = N * (N + 1);
    int completed_row[N];
    cmd_t cmd;
    char p_free[P];
    int i, j;

    generate_cmd_array(multiply_cmd_array, sum_cmd_array, N);

    for (i = 0; i < P; ++i) {
        p_free[i] = 1;
    }

    for (i = 0; i < N; i++) {
        completed_row[i] = 0;
    }

    i = j = 0;

    while (number_of_cmd >= 1) {
        int p;
        if ((p = first_free(p_free, P)) != -1) {
            // ci sono processi liberi
            if ((i < N) && completed_row[sum_cmd_array[i].data.row] == N) {
                // se ci sono ancora comandi di tipo somma e sono terminate le moltiplicazioni per quella riga
                send_cmd(&sum_cmd_array[i], pid_to_pipe[p], p, sem_ids->pipe_sem);
                i++;
                p_free[p] = 0;
            } else if ((j < (N * N))) {
                // a questo punto o ho finito le moltiplicazioni o non ho finito le moltiplicazioni per la riga
                send_cmd(&multiply_cmd_array[j], pid_to_pipe[p], p, sem_ids->pipe_sem);
                j++;
                p_free[p] = 0;
            } else {
                // ho finito le moltiplicazioni ma non mi è ancora arrivato il riscontro di conseguenza non posso
                // aver finito, per non creare un ciclo infinito occupo il processo ed aspetto fino a quando non 
                // finisce uno di quelli impegnati nelle ultime moltiplicazioni. 
                p_free[p] = 0; 
            }
        } else {
            msg_t msg;
            rcv_msg(&msg, queue);
            if (msg.success) {
                p_free[msg.id] = 1;
                number_of_cmd--;
                if (msg.cmd.role == MULTIPLY) {
                    completed_row[msg.cmd.data.c.i]++;
                }
            } else {
                send_cmd(&msg.cmd, pid_to_pipe[msg.id], p, sem_ids->pipe_sem);
            }
        }
    }

    // ho finito tutti i messaggi da inviare ed ho ricevuto il riscontro da tutti.
    sys_print(STDOUT, "--- Ending ---\n\n");
    for (int p = 0; p < P; ++p) {
        cmd.role = END;
        send_cmd(&cmd, pid_to_pipe[p], p, sem_ids->pipe_sem);
    }

    return 0;
}


static inline int first_free(char *a, int dim) {
    for (int i = 0; i < dim; ++i)
        if (a[i] == 1) return i;
    return -1;
}

void generate_cmd_array(cmd_t *multiply_array, cmd_t *sum_array, int N){
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            multiply_array[i * N + j].role = MULTIPLY;
            multiply_array[i * N + j].data.c.i = i;
            multiply_array[i * N + j].data.c.j = j;
        }
        sum_array[i].role = SUM;
        sum_array[i].data.row = i;
    }
}