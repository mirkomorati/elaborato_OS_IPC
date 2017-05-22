#include "../headers/father.h"

// dichiaro alcune funzioni statiche 

static inline int first_free(char *a, int dim);


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

    shm_t A, B, C, S;
    shm_t *shm_array[5] = {&A, &B, &C, &S, NULL};
    int N;
    int P;
    lock_t sem_ids;
    int queue_id;

    if (argc < 10) {
        char *buf = "Error: too few arguments.\n\
        usage: ./elaborato_IPC -A matrix -B matrix -C matrix -N order -P #processes\n";
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
                    if(creat(optarg, S_IRUSR | S_IWUSR) == -1)
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

    int pid_to_pipe[P];

    A.N = N;
    B.N = N;
    C.N = N;
    S.N = 1;
    S.path = "/dev/urandom";

    if(init(shm_array, &sem_ids, P) == -1) {
        perror("init");
        sig_end(-1);
    }

    if (make_child(shm_array, &sem_ids, P, pid_to_pipe, &queue_id) == -1){
        perror("make_child");
        sig_end(-1);
    }

    if (run(N, P, pid_to_pipe, queue_id, &sem_ids) == -1) {
        perror("run");
        sig_end(-1);
    }

    int status, wpid;
    while((wpid = wait(&status)) > 0)
        printf("terminazione normale: il figlio %i ha terminato\n", wpid);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%li\t", C.shmaddr[i * N + j]);
        }
        printf("\n");
    } 
    printf("\n");

    shmatrix_to_csv(&C);

    printf("--- SOMMA: %li ---\n", S.shmaddr[0]);

    sig_free_sem(false, NULL);
    sig_free_memory(false, NULL);
    sig_free_queue(false, NULL);
    exit(0);
}


int init(shm_t **shm_array, lock_t *sem_ids, int P) {
    #ifdef DEBUG   
    printf("Loading matrix\n\n");
    #endif

    if (shm_load(shm_array[0], true) == -1) {
        perror("shm_load A");
        return -1;
    }
    if (shm_load(shm_array[1], true) == -1) {
        perror("shm_load B");
        return -1;
    }
    if (shm_load(shm_array[2], false) == -1) {
        perror("shm_load C");
        return -1;
    }
    if (shm_load(shm_array[3], false) == -1) {
        perror("shm_load S");
        return -1;
    }
    shm_array[3]->shmaddr[0] = 0;
    

    if ((sem_ids->queue_sem = sem_create(1, 1)) == -1) return -1;
    if ((sem_ids->S_sem = sem_create(1, 1)) == -1) return -1;
    if ((sem_ids->pipe_sem = sem_create(P, 0)) == -1) return -1;

    return 0;
}


int make_child(shm_t **shm_array , lock_t *sem_ids, int P, int *pid_to_pipe, int *queue_id) {
    int *tmp_pipe[P];
    int tmp_queue_id;
    int pids[P];
    #ifdef DEBUG
    printf("creating pipes\n");
    #endif
    for (int i = 0; i < P; ++i){
        tmp_pipe[i] = (int *) malloc(2*sizeof(int));
        if(pipe(tmp_pipe[i]) == -1){
            perror("ERROR make_child - creating pipe");
            return -1;
        }
    }
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
            exit(child(i, shm_array, tmp_pipe[i][0], tmp_queue_id, sem_ids));
        }else{
            pid_to_pipe[i] = tmp_pipe[i][1];
        }
    }

    #ifdef DEBUG
    printf("\n%i childs created:\n", P);
    for (int i = 0; i < P; ++i){
        printf("child %i:\tpid = %i\tpipe = (r:%i,w:%i)\n",i, pids[i], tmp_pipe[i][0], tmp_pipe[i][1]);
    }
    printf("\n");
    #endif

    for (int i = 0; i < P; ++i){
        free(tmp_pipe[i]);
    }
    return 0;   
}


int run(int N, int P, int *pid_to_pipe, int queue, lock_t *sem_ids) {
    cmd_list_t *cmd_list = NULL; // attenzione gestione della lista errata.
    int number_of_cmd = generate_cmd_list(&cmd_list, N);
    int executed_cmds = 0;
    int completed_row[N];
    cmd_t cmd;
    char p_free[P];
    for (int i = 0; i < P; ++i) {
        p_free[i] = 1;
    }

    for (int i = 0; i < N; i++) {
        completed_row[i] = 0;
    }

    printf("inizio il ciclo di base di run\n");
    while(executed_cmds < number_of_cmd && cmd_list != NULL){
        int p;
        if ((p = first_free(p_free, P))!= -1){
            printf("invio comando %s al figlio %i\n",cmd_list->cmd.role == MULTIPLY ? "MULTIPLY" : "SUM", p);
            // ci sono processi liberi
            
            // NON FUNZIONA, l'idea e' di aggiungere una sum non eseguibile perche' non sono finite le multiply su quella riga in coda alla lista
            if (cmd_list->cmd.role == SUM && completed_row[cmd_list->cmd.data.row] != N) {
                printf("Aggiungo SUM %i in coda, completed: %i\n", cmd_list->cmd.data.row, completed_row[cmd_list->cmd.data.row]);
                add_to_cmd_list(&cmd_list, &cmd_list->cmd);
                cmd_list = cmd_list->next;
                continue;
            }
            
            send_cmd(&cmd_list->cmd, pid_to_pipe[p], p, sem_ids->pipe_sem);
            cmd_list = cmd_list->next;
            p_free[p] = 0;
        } else {
            msg_t msg;
            rcv_msg(&msg, queue);
            if (msg.success) {
                p_free[msg.id] = 1;
                executed_cmds++;
            } else {
                send_cmd(&msg.cmd, pid_to_pipe[msg.id], p, sem_ids->pipe_sem);
            }
            if (msg.cmd.role == MULTIPLY) {
                completed_row[msg.cmd.data.c.i]++;
                printf("i: %i, completed: %i\n", msg.cmd.data.c.i, completed_row[msg.cmd.data.c.i]);
            }
        }
    }

    // ho finito tutti i messaggi da inviare ed ho ricevuto il riscontro da tutti.
    for (int p = 0; p < P; ++p){
        cmd.role = END;
        send_cmd(&cmd, pid_to_pipe[p], p, sem_ids->pipe_sem);
    }
    return 0;
}

/*
int run(int N, int P, pid_to_pipe_t *pid_to_pipe, int queue) {
    int completed_rows[N];
    for (int i = 0; i < N; ++i)
        completed_rows[i] = 0;

    cmd_t cmd;
    msg_t msg;

    int i = 0, j = 0, errors = 0;
    
    for (int p = 0; p < P; ++p) {
        cmd.role = MULTIPLY;
        cmd.data.c.i = i;
        cmd.data.c.j = j;
        if (send_cmd(&cmd, pid_to_pipe[p].pipe_fd) == -1) {
            // QUI SE IL NUMERO DI PROCESSI È MAGGIORE DEL NUMERO DI RIGA È NECESSARIO INVIARE ANCHE UNA SOMMA.
            if(++errors > MAX_ERRORS) {
                perror("too many errors");
                return -1;
            } else {
                printf("comando non mandato a: %i, su pipe %i\n", pid_to_pipe[p].pid, pid_to_pipe[p].pipe_fd);
                j--; // da tenere così.
                j %= N;
                i -= j / N;
            }
        } else if (p != P - 1) {
            j++;
            i += j / N;
            j %= N;
        }
    }

    errors = 0;


    while(true){

        if (i == N) { 
            printf("waiting for ctrl-c...\n");
            usleep(2e6);
            cmd.role = END;
            for (int p = 0; p < P; ++p)
            {
                if (send_cmd(&cmd, pid_to_pipe[p].pipe_fd) == -1) {
                    perror("sending end cmd");
                    printf("comando non mandato a: %i, su pipe %i\n", pid_to_pipe[p].pid, pid_to_pipe[p].pipe_fd);
                } else{
                    int status;
                    waitpid(pid_to_pipe[p].pid, &status, 0);
                    printf("inviato END al figlio: %i sulla pipe: %i\n", pid_to_pipe[p].pid, pid_to_pipe[p].pipe_fd);
                }
            }
            break;
        }

        if (rcv_msg(&msg, queue) == -1) {
            if(++errors > MAX_ERRORS) {
                perror("too many errors");
                return -1;
            }
        }

        int pipe;
        for (int p = 0; p < P; ++p)
            if (msg.pid == pid_to_pipe[p].pid)
                pipe = pid_to_pipe[p].pipe_fd;

        if(msg.success) {

            pending--;
            if(msg.cmd.role == MULTIPLY && ++completed_rows[msg.cmd.data.c.i] == N) {
                cmd.role = SUM;
                cmd.data.row = msg.cmd.data.c.i;
            } else {
                j++;
                i += j / N;
                j %= N;
                cmd.role = MULTIPLY;
                cmd.data.c.i = i;
                cmd.data.c.j = j;
            }

            if (i == N)
                continue;

            if (send_cmd(&cmd, pipe) == -1) {
                printf("comando non mandato a: %i, su pipe %i\n", msg.pid, pipe);
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
            if (send_cmd(&msg.cmd, pipe) == -1) {
                if(++errors > MAX_ERRORS) {
                    perror("too many errors");
                    return -1;
                } 
            }
        }
    }

    return 0;
}
*/
static inline int first_free(char *a, int dim){
    for (int i = 0; i < dim; ++i)
        if (a[i] == 1) return i;
    return -1;
}

int generate_cmd_list(cmd_list_t **head, int N){
    cmd_t cmd;
    int cmd_number = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cmd.role = MULTIPLY;
            cmd.data.c.i = i;
            cmd.data.c.j = j;
            add_to_cmd_list(head, &cmd);
            cmd_number++;
        }
    }

    for (int i = 0; i < N; i++) {
        cmd.role = SUM;
        cmd.data.row = i;
        add_to_cmd_list(head, &cmd);
        cmd_number++;
    }
    
    return cmd_number;
}

void add_to_cmd_list(cmd_list_t **head, cmd_t *cmd){
    cmd_list_t *tmp = *head;
    if(tmp == NULL){
        // creo la lista
        tmp = (cmd_list_t *)malloc(sizeof(cmd_list_t));
        tmp->cmd = *cmd;
        tmp->next = NULL;
        *head = tmp;
    }else{ // la riempio
        while(tmp->next != NULL)
            tmp = tmp->next;

        tmp->next = (cmd_list_t *)malloc(sizeof(cmd_list_t));
        tmp = tmp->next;
        tmp->cmd = *cmd;
        tmp->next = NULL;
    }
}