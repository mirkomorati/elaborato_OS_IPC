/*!
 * @file child.h 
 * 
 * Contiene le funzioni eseguibili dai processi figli per
 * manipolare l'aritmetica sulle matrici.
 */

#ifndef CHILD_H
#define CHILD_H

#include "std_lib.h"
#include "shm_lib.h"
#include "sem_lib.h"
#include "msg_lib.h"
#include "io_lib.h"

/*!
 * @brief     	Rappresenta il "main" di ogni figlio. N.B da questa funzione o da
 *             	qualsiasi funzione chiamata da questa, non si deve mai usare
 *             	sig_end(), sig end deve essere chiamata solo e soltanto dal
 *             	padre.
 *
 * @param[in]  	child_id   	ID del figlio.
 * @param[in]  	shm_array  	L'array contente le zone di memoria condivise.
 * @param[in]  	pipe_fd    	La pipe sulla quale leggere.
 * @param[in]  	queue_id   	La coda sulla quale scrivere.
 * @param[in]  	sem_ids    	ID dei semafori.
 *
 * @return     	0 in caso di successo, -1 altrimenti.
 */
int child(int child_id, shm_t **shm_array, int pipe_fd, int queue_id, lock_t *sem_ids);

/*!
 * @brief      	Funzione che calcola il prodotto tra due righe di due matrici e lo salva in
 *             	posizione i j nella matrice destinazione
 *
 * @param[in]  	i          	Numero di riga
 * @param[in]  	j          	Numero di colonna
 * @param      	shm_array 	Array contenente i riferimenti alle zone di memoria condivisa.
 */
void multiply(int i, int j, shm_t **shm_array);

/*!
 * @brief      	Funzione che calcola la somma di una riga ed aggiunge il risultato
 *             	al valore contenuto nella zona di memoria predefinita
 *
 * @param[in]  	k          	Numero di riga
 * @param      	shm_array  	Array contenente i riferimenti alle zone di memoria condivisa.
 * @param[in]  	sem_ids    	ID dei semafori
 *
 * @return     	0 in caso di successo, -1 altrimenti.
 */
int sum(int k, shm_t **shm_array, lock_t *sem_ids);

#endif