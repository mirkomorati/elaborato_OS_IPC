#ifndef CHILD_H
#define CHILD_H

#include "std_lib.h"
#include "shm_lib.h"
#include "msg_lib.h"

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
int child(shm_t **shm_array, int pipe_fd, int queue_id, int *sem_id_array);

#endif