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
	int A_sem;
	int B_sem;
	int result_sem;
} lock_t;

/*!
 * \brief      	Raccoglie le istruzioni per decrementare il semaforo id
 * 				di 1 e attendere nel caso in cui non il suo valore sia 
 * 				minore di 0.
 *
 * \param[in]  	id		L'identificatore del set.
 * \param[in]	nsem	Il semaforo del set su cui effettuare il lock
 */
int sem_lock(int id, int nsem);

/*!
 * \brief      	Raccoglie le istruzioni necessarie all'incremento del valore
 * 				del semaforo id di 1.
 *
 * \param[in]  	id    	L'identificatore del set.
 * \param[in]	nsem	Il semaforo del set su cui effettuare il lock
 */
int sem_unlock(int id, int nsem);

/*!
 * \brief      	Raccoglie le operazioni necessarie alla creazione di un nuovo
 * 				set di semafori inizializzato ad 1.
 * 				
 * N.B. La funzione toglie molte scelte al programmatore, ma dato che nella 
 * 		maggior parte del nostro programma useremo set di 1 semaforo inizializzato
 * 		ad 1, la funzione farà esattamente questo.
 * 		
 * \param[in]	Il numero di semafori che devono essere contenuti nel set
 *
 * \return     	L'id del semaforo creato. -1 in caso di errore
 */
int sem_create(int nsem);

#endif