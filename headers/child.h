#ifndef CHILD_H
#define CHILD_H

#include "std_lib.h"
#include "shm_lib.h"
#include "sem_lib.h"
#include "msg_lib.h"
#include "io_lib.h"

/*!
 * \brief      	Rappresenta il "main" di ogni figlio. N.B da questa funzione o da qualsiasi 
 * 				funzione chiamata da questa, non si deve mai usare sig_end(), sig end deve 
 * 				essere chiamata solo e soltanto dal padre.
 *
 * \param      	shm_array  	L'array contente le zone di memoria condivise.
 * \param[in]  	pipe_fd    	La pipe sulla quale leggere.
 * \param[in]  	queue_id   	La coda sulla quale scrivere.
 * 
 * \return		0 in caso di successo, -1 altrimenti.
 */
int child(int child_id, shm_t **shm_array, int pipe_fd, int queue_id, lock_t *sem_ids);

/*!
 * @brief       Funzione che calcola il prodotto tra due matrici e lo
 *              salva in posizione i j nella matrice destinazione     
 *
 * @param[in]  i            Numero di riga
 * @param[in]  j            Numero di colonna
 */
void multiply(int i, int j, shm_t **shm_array);

/*!
 * @brief       Funzione che calcola la somma di una riga e aggiorna il
 *              valore nell'area di memoria predefinita
 *
 * @param[in]  k     { parameter_description }
 * 
 * \return      0 in caso di successo, -1 altrimenti.
 */
int sum(int k, shm_t **shm_array, lock_t *sem_ids);

#endif