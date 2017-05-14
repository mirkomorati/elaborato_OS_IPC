/*!
 * \file        std_lib.h
 * Contiene tutti gli header delle librerie C utili al progetto
 * specificandone l'utilizzo. Possiamo includere direttamente questo
 * file in tutti i file dato che tanto verrà incluso una volta sola
 * grazie alla ifndef iniziale.
 */

#ifndef STD_LIB_H
#define STD_LIB_H

#include <stdio.h>		// per perror.
#include <stdlib.h>	
#include <unistd.h>		// malloc etc.
#include <stdarg.h>		// per i var arg.
#include <fcntl.h>		// controllo di file e memorie condivise.
#include <getopt.h>		// per le opzioni.
#include <string.h>		// per stampare a video con semplicità
#include <signal.h>		// gestione dei segnali come SIGINT
#include <sys/shm.h>	// per la memoria condivisa.
#include <sys/types.h>
#include <sys/msg.h> 	// per le code di messaggi.
#include <sys/sem.h> 	// gestione dei semafori.
#include <sys/ipc.h>  	// gestione di tutto cio che riguarda la comunicazione tra i processi.
#include <sys/stat.h> 	// funzione stat per ottenere lo status di un file.
#include <stdbool.h>	// per avere i booleani in C.
#include <errno.h>		// gestione degli errori, vedesi ENOENT.

#endif