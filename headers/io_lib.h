/*!
 * \file io_lib.h
 */

#ifndef IO_LIB_H
#define IO_LIB_H
#include "std_lib.h"
#include "shm_lib.h"

/**
 * @brief      Conversione di una matrice salvata come shared memory
 *             in formato csv e salvataggio su file
 *
 * @param      M     La matrice da convertire
 */     
void shmatrix_to_csv(shm_t *M);

#endif