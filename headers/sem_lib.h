/*!
 * \file        sem_lib.h
 * Contiene i prototipi di alcune funzioni che risultano utili
 * nell'utilizzo dei semafori
 */

#ifndef SEM_LIB_H
#define SEM_LIB_H
#include "std_lib.h"

typedef struct{
	int queue_sem;
    int S_sem;
    int pipe_sem;
} lock_t;

/*!
 * \brief       Raccoglie le operazioni necessarie alla creazione di un nuovo
 *              set di semafori inizializzato al valore di init
 *      
 * \param[in]   nsem    Il numero di semafori che devono essere contenuti nel set
 * \param[in]   init    Valore iniziale del semaforo
 * 
 * \return      L'id del semaforo creato. -1 in caso di errore
 */
int sem_create(int nsem, int init);

/*!
 * \brief      	Raccoglie le istruzioni per decrementare il semaforo id
 * 				di 1 e attendere nel caso in cui non il suo valore sia 
 * 				minore di 0.
 *
 * \param[in]  	id		L'identificatore del set.
 * \param[in]	nsem	Il semaforo del set su cui effettuare il lock
 *
 * \return      0 in caso di successo, -1 altrimenti
 */
int sem_lock(int id, int nsem);

/*!
 * \brief      	Raccoglie le istruzioni necessarie all'incremento del valore
 * 				del semaforo id di 1.
 *
 * \param[in]  	id    	L'identificatore del set.
 * \param[in]	nsem	Il semaforo del set su cui effettuare il lock
 *
 * \return      0 in caso di successo, -1 altrimenti
 */
int sem_unlock(int id, int nsem);

/*!
 * \brief       Alias per sem_unlock per semafori interi
 *
 * \param[in]   id      L'identificatore del set
 * \param[in]   nsem    Il semaforo del set su cui effettuare l'incremento
 *
 * \return      0 in caso di successo, -1 altrimenti
 */
int sem_inc(int id, int nsem);

/*!
 * \brief       Alias per sem_lock per semafori interi
 *
 * \param[in]   id      L'identificatore del set
 * \param[in]   nsem    Il semaforo del set su cui effettuare il decremento
 *
 * \return      0 in caso di successo, -1 altrimenti
 */
int sem_dec(int id, int nsem);
#endif