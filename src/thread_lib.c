#include "../headers/thread_lib.h"

int use_thread(char *A_path, char *B_path, char *C_path, int N) {
    thread_arg_t args[N+1];
    pthread_t threads[N+1];
    long *matrixA, *matrixB, *matrixC, *sum;
    int *completed_rows;
    pthread_mutex_t sum_mutex; 

    int A_fd, B_fd, C_fd;

    if ((A_fd = open(A_path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("ERROR use_thread - open");
        return -1;
    }

    if ((B_fd = open(B_path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("ERROR use_thread - open");
        return -1;
    }

    if ((C_fd = open(C_path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("ERROR use_thread - open");
        return -1;
    }

    if (pthread_mutex_init(&sum_mutex, NULL)){
        sys_err("ERROR use_thread - mutex init");
    }

    matrixA = (long *) malloc(sizeof(long) * N * N);
    matrixB = (long *) malloc(sizeof(long) * N * N);
    matrixC = (long *) malloc(sizeof(long) * N * N);
    completed_rows = (int *) malloc(sizeof(int) * N);
    sum = (long *) malloc(sizeof(long));


    matrix_from_csv(A_fd, matrixA, N);
    matrix_from_csv(B_fd, matrixB, N);

    for (int i = 0; i < N+1; ++i) {
        args[i].dimension = N;
        args[i].matrixA = matrixA;
        args[i].matrixB = matrixB;
        args[i].matrixC = matrixC;
        args[i].role = (i == N) ? T_SUM : T_MULTIPLY;
        args[i].row = (i < N) ? i : 0;
        args[i].sum_mutex = &sum_mutex;
        args[i].completed_rows = completed_rows;
        args[i].sum = sum;
    }


#ifdef DEBUG
    if (N <= 20) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                sys_print(STDOUT, "%i\t", matrixA[i * N + j]);
            }
            sys_print(STDOUT, "\n");
        }
        sys_print(STDOUT, "\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                sys_print(STDOUT, "%i\t", matrixB[i * N + j]);
            }
            sys_print(STDOUT, "\n");
        }
    }
#endif


    if (create_threads(threads, args, N + 1) == -1){
        sys_err("error creatin threads in use_thread");
        return -1;
    }

    for (int i = 0; i < N + 1; ++i){
        pthread_join(threads[i], NULL);
    }

    matrix_to_csv(C_fd, matrixC, N);

    free(matrixA);
    free(matrixB);
    free(matrixC);
    free(completed_rows);
    free(sum);
    return 0;
}

void matrix_to_csv(int fd, long *matrix, int N) {
    char buf[BUF_SIZE];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int bytes = sprintf(buf, (j + 1 == N ? "%li\n" : "%li,"), matrix[i * N + j]);
            if (write(fd, buf, bytes) != bytes) {
                sys_err("ERROR matrix_to_csv - write");
            }
        }
    }    
}

void matrix_from_csv(int fd, long *matrix, int N) {
    char *buf;
    int i = 0;
    char *line, *value, *brkt, *brkb;

    int curr = lseek(fd, 0, SEEK_CUR);
    int buf_size = lseek(fd, 0, SEEK_END);
    lseek(fd, curr, SEEK_SET);
    buf = (char *) malloc(sizeof(char) * buf_size + 1);

    if (read(fd, buf, buf_size) != 0) {
        buf[buf_size] = '\0';
        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {
            for (value = strtok_r(line, ",", &brkb); value; value = strtok_r(NULL, ",", &brkb)) {
                matrix[i] = atol(value);
                i++;
            }
        }
    }

    free(buf);
}

int create_threads(pthread_t *thread_array, thread_arg_t *args, int thread_number){
    for(int i = 0; i < thread_number; i++){
        if(pthread_create(&thread_array[i], NULL, thread_callback, (void *) (&args[i]) )){
            sys_err("error creating threads");
            return -1;
        }
    }

    return 0;
}

void * thread_callback(void * args){
    thread_arg_t *arg = (thread_arg_t *) args; 
    int N = arg->dimension; // per comodità nel codice.
    sys_print(STDOUT, "THREAD ROLE: %s\n", arg->role == T_SUM ? "SUM" : "MULTIPLY");

    switch (arg->role){
        case T_SUM: {
            *(arg->sum) = 0;
            int completed = 0;
            int done[N];
            for (int i = 0; i < N; ++i) {
                done[i] = 0;
            }
            while(completed < N) {
                for (int i = 0; i < N; ++i) {
                    pthread_mutex_lock(arg->sum_mutex);
                    if (arg->completed_rows[i] == 1 && done[i] == 0) {
                        for (int j = 0; j < N; ++j){
                            *(arg->sum) += arg->matrixC[(i * N) + j];
                        }
                        completed++;
                        done[i] = 1;
                    }   
                    pthread_mutex_unlock(arg->sum_mutex);
                }
            }
            #ifdef DEBUG
            sys_print(STDOUT, "SUM: %li\n", *(arg->sum));
            #endif
        }
        break;
        
        case T_MULTIPLY : {
            long *row = (long *) malloc(sizeof(long) * N);
            long *col = (long *) malloc(sizeof(long) * N);
            long res = 0;

            for (int j = 0; j < N; j++) {
                res = 0;
                for (int l = 0; l < N; l++) {
                    row[l] = arg->matrixA[arg->row * N + l];
                    col[l] = arg->matrixB[j + l * N];
                }

                for (int l = 0; l < N; l++) {
                    res += row[l] * col[l];
                }

                arg->matrixC[arg->row * N + j] = res;
                #ifdef DEBUG
                sys_print(STDOUT, "MULTIPLY row %i col %i:\t%li\n", arg->row, j, res);
                #endif
            }
            arg->completed_rows[arg->row] = 1;
            free(row);
            free(col);
        }
        break;

        default:
            sys_err("role not recognized!");
    }
    return NULL;
}