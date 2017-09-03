#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "NumRecipe.h"

/**
 * NOTE: All code in this file was taken from the book:
 *
 * Numerical Recipes in C: The Art of Scneitific Computing
 * (c) 1988-1992 Cambridge University Press
 *
 * Some code was modified slightly to fit with the calling program.
 */

/**
 * Allocate a double matrix with subscript range 
 * m[nrl..nrh][ncl..nch] 
 */
double **matrix (long nrl, long nrh, long ncl, long nch)
{
  long i, nrow = nrh-nrl+1, ncol = nch-ncl+1;
  double **m;

  /* allocate pointers to rows */
  m = (double**)malloc((size_t)((nrow)*sizeof(double*)));
  if (!m) printf("allocation failure 1 in matrix()");
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl] = (double*)malloc((size_t)((nrow*ncol)*sizeof(double)));
  if (!m[nrl]) printf("allocation failure 2 in matrix()");
  m[nrl] -= ncl;
  for (i=nrl+1;i<nrh;i++) m[i] = m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}

/** Free a double matrix allocated by matrix() */
void free_matrix (double **m, long nrl, long nrh, long ncl, long nch)
{
  free((char*)(m[nrl]+ncl));
  free((char*)(m+nrl));
}

/**
 * Allocate an int matrix with subscript range 
 * m[nrl..nrh][ncl..nch] 
 */
int **i_matrix (long nrl, long nrh, long ncl, long nch)
{
  long i, nrow = nrh-nrl+1, ncol = nch-ncl+1;
  int **m;

  /* allocate pointers to rows */
  m = (int**)malloc((size_t)((nrow)*sizeof(int*)));
  if (!m) printf("allocation failure 1 in matrix()");
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl] = (int*)malloc((size_t)((nrow*ncol)*sizeof(int)));
  if (!m[nrl]) printf("allocation failure 2 in matrix()");
  m[nrl] -= ncl;
  for (i=nrl+1;i<nrh;i++) m[i] = m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}

/** Free an int matrix allocated by matrix() */
void free_i_matrix (int **m, long nrl, long nrh, long ncl, long nch)
{
  free((char*)(m[nrl]+ncl));
  free((char*)(m+nrl));
}

/**
 * Allocate a double matrix with subscript range 
 * m[nrl..nrh][ncl..nch] 
 */
long **l_matrix (long nrl, long nrh, long ncl, long nch)
{
  long i, nrow = nrh-nrl+1, ncol = nch-ncl+1;
  long **m;

  /* allocate pointers to rows */
  m = (long**)malloc((size_t)((nrow)*sizeof(long*)));
  if (!m) printf("allocation failure 1 in matrix()");
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl] = (long*)malloc((size_t)((nrow*ncol)*sizeof(long)));
  if (!m[nrl]) printf("allocation failure 2 in matrix()");
  m[nrl] -= ncl;
  for (i=nrl+1;i<nrh;i++) m[i] = m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}

/** Free a double matrix allocated by matrix() */
void free_l_matrix (long**m, long nrl, long nrh, long ncl, long nch)
{
  free((char*)(m[nrl]+ncl));
  free((char*)(m+nrl));
}
