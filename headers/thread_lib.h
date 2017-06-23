/* file che contiene tutte le utility relative al ai thread */
#include <pthread.h>

typedef enum{
	SUM,
	MULTIPLY
}role_t;

typedef struct{
	mutex_t sum_sem_mutex;
	role_t role;
	int row;
	long *matrixA;
	long *matrixB;
	long *matrixC;
}thread_arg_t;

/**
 * @brief      Crea thread_number thread passando ad ogni thread gli argomenti corretti.
 *
 * @param      thread_array  	L'array dove vengono salvati gli oggetti di tipo thread_t.
 * @param      args          	L'array contnente alla posizione i gli argomenti da passare all'array i
 * @param[in]  thread_number  	Il numero dei thread
 *
 * @return     0 in caso di successo -1 altrimenti.
 */
int create_threads(pthread_t *thread_array, thread_arg_t *args, int thread_number);


