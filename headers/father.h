#ifndef FATHER_H
#define FATHER_H
#include "std_lib.h"
#include "shm_lib.h"
#include "ending_lib.h"
#include "sem_lib.h"
#include "msg_lib.h"
#include "child.h"

#define MAX_ERRORS 3
/*!
 * \brief		Funzione che inizializza la memoria condivisa.
 *
 * \param      	shm_array  L'array delle memorie condivise da creare.
 *
 * \return     	0 in caso di successo, -1 altrimenti.
 */
int init(shm_t **shm_array);

/**
 * \brief      	Crea P figli del padre ed inizializza gli strumenti per permettere
 * 				la comunicazione tra padre e figli.
 *
 * \param[in]  	shm_array  	L'array contenente i puntatori alle zone di memoria condivisa.
 * \param[in]  	P			Il numero di figli da creare.
 * \param[out] 	pipe_fd  	In questo puntatore viene salvato il file descriptor per scrivere	
 * 							sulla pipe.
 * \param[out] 	queue_id   	In questo indirizzo viene salvato l'identificativo della coda da cui
 * 							leggere le comunicazioni dei figli.
 *
 * \return     	0 in caso di riuscita, -1 altrimenti.
 */
int make_child(shm_t **shm_array, int P, int *pipe_fd, int *queue_id);

/*!
 * \brief      	La funzione si occupa di tutto quello che avviene nel core del programma: 
 * 				effettua un loop in cui continua a chiedere ai figli di fare un lavoro
 * 				e controlla che lo abbiano svolto correttamente, se un figli ci riesce allora,
 * 				il padre legge il risultato ed invia un nuovo comando.
 *
 * \param[in]	N		Ordine delle matrici da moltiplicare.
 * \param[in]  	P      	Il numero di figli sui quali deve ciclare.
 * \param[in]  	pipe   	La pipe sulla quale deve scrivere.
 * \param[in]  	queue  	La coda dalla quale deve leggere.
 *
 * \return     	Il codice di uscita del programma.
 */
int run(int N, int P, int pipe, int queue);

#endif