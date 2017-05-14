/*!
 * \file        sem_lib.h
 * Contiene i prototipi di alcune funzioni che risultano utili
 * nell'utilizzo dei semafori
 */

#ifndef SEM_LIB_H
#define SEM_LIB_H
#include "std_lib.h"

/*!
 * \brief      	Raccoglie le istruzioni per decrementare il semaforo id
 * 				di 1 e attendere nel caso in cui non il suo valore sia 
 * 				minore di 0.
 *
 * \param[in]  	id		L'identificatore del semaforo.
 */
void sem_lock(int id);

/*!
 * \brief      	Raccoglie le istruzioni necessarie all'incremento del valore
 * 				del semaforo id di 1.
 *
 * \param[in]  id    	L'identificatore del semaforo.
 */
void sem_unlock(int id);

/*!
 * \brief      	Raccoglie le operazioni necessarie alla creazione di un nuovo
 * 				semaforo inizializzato ad 1.
 * 				
 * N.B. La funzione toglie molte scelte al programmatore, ma dato che nella 
 * 		maggior parte del nostro programma useremo set di 1 semaforo inizializzato
 * 		ad 1, la funzione farà esattamente questo.
 *
 * \return     	L'id del semaforo creato.
 */
int sem_create();

#endif