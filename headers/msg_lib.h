/*! \file msg_utils.h
 * Raccolta di funzioni e strutture dati per lo scambio di messaggi.
 * 
 * Conterrà tutte le funzioni ed i dati di
 * utilità per i messaggi scambiati tra padre e figli 
 * o viceversa attraverso code di messaggi e pipe
 */
#ifndef MSG_UTILS_H
#define MSG_UTILS_H
#include <stdbool.h>
#include <stdint.h>

/*! \brief Rappresenta il ruolo di un figlio.
 * 
 * Il ruolo è variabile ed è parte di un comando 
 * che può essere inviato da un padre ad un figlio.
 * Può essere:
 * 	- MULTIPLY: il figlio deve effettuare una moltiplicazione.
 * 	- SUM : il figlio deve effettuare una somma.
 */
typedef enum{
	MULTIPLY, 
	SUM
}role_t;

/*! \brief Comando inviato dal padre ai figli.
 * 
 * Rappresenta un comando inviato dal padre ad uno dei suoi figli.
 */
typedef struct{
	role_t role; //!< Ruolo che il figlio deve svolgere.
	union data {
		struct point{
			uint32_t i;	
			uint32_t j;
		} c;
		uint64_t row;
	} x; //!< \brief Dati del comando. 
	     //! In caso di somma va tenuto conto della
	     //! valore di row, altrimenti va tenuto conto del valore di c.
}cmd_t;

/*! \brief Messaggio dei figli al padre 
 * 
 * Rappresenta un messaggio per la coda di messaggi utilizzata dai 
 * figli per comunicare con il padre.
 */
typedef struct{
	long type;			//!< Tipo del messaggio.
	bool success;		//!< True se il comando è stato eseguito correttamente.
	cmd_t cmd;			//!< Comando che doveva essere eseguito.
}msg_t;


/*!
 * \brief      	Mette il messaggio msg nella coda di messaggi che devono essere letti
 * 				dal padre. L'operazione è __safe__.
 *
 * \param[in]  	msg   	Il messaggio.
 * \param[in]	id 		Se diverso da NULL modifica l'id della coda. 
 * 						viene utilizzato in fase di setting.
 *
 * \return     	0 in caso di successo, -1 altrimenti.
 */
int send_msg(const msg_t * restrict msg, const int * restrict id);


/*!
 * \brief      	Preleva un messaggio dalla coda di messaggi diretti al padre.
 * 				L'operazione è __safe__.
 *
 * \param[out]  msg   	Zona di memoria in cui memorizzare il messaggio.
 * \param[in]	id 		Se diverso da NULL modifica l'id della coda. 
 * 						viene utilizzato in fase di setting.
 *
 * \return     	0 in caso di successo, -1 altrimenti.
 */
int rcv_msg(msg_t * restrict msg, const int * restrict id);

/*!
 * \brief      	Scrive un comando nella pipe che collega il padre con i figli.
 *				L'operazione è __safe__.
 *				
 * \param[in]  	cmd		Il comando.
 * \param[in] 	id    	Se diverso da NULL modifica il file descriptor della pipe.
 * 						viene usato in fase di setting.
 *
 * \return    	0 in caso di successo, -1 altrimenti.
 */
int send_cmd(const cmd_t * restrict cmd, const int * restrict id);

/*!
 * \brief      	Legge un comando scritto dal padre nella pipe che lo collega ai figli
 * 
 * \param[out]  cmd		Zona di memoria in cui memorizzare il comando.
 * \param[in]  	id    	Se diverso da NULL modifica il file descriptor della pipe.
 * 						viene usato in fase di setting.
 *
 * \return    	0 in caso di successo, -1 altrimenti.
 */
int rcv_cmd(cmd_t * restrict cmd, const int * restrict id);
#endif