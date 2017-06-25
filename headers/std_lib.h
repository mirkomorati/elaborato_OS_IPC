/*!
 * \file        std_lib.h
 * 
 * Contiene tutti gli header delle librerie C utili al progetto
 * specificandone l'utilizzo. Possiamo includere direttamente questo
 * file in tutti i file dato che tanto verrà incluso una volta sola
 * grazie alla ifndef iniziale.
 */

#ifndef STD_LIB_H
#define STD_LIB_H

#include <stdio.h>      // sprintf
#include <stdlib.h>     // exit, atoi, malloc, free
#include <unistd.h>     // write, read, fork, getpid, pipe
#include <signal.h>     // signal
#include <stdarg.h>		// va_start, va_arg, va_end
#include <stdbool.h>    // booleani
#include <errno.h>      // errno
#include <fcntl.h>		// creat, open
#include <getopt.h>		// option
#include <string.h>		// strlen, strtok_r, strerror
#include <sys/shm.h>	// shmat, shmdt, shmctl, shmget
#include <sys/msg.h> 	// msgget, msgctl, msgrcv
#include <sys/sem.h> 	// semget, semctl, semop
#include <sys/stat.h> 	// stat
#include <sys/types.h>
#include <sys/ipc.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#endif