#ifndef __numRecipe
#define __numRecipe

double **matrix (long nrl, long nrh, long ncl, long nch);
void free_matrix (double **m, long nrl, long nrh, long ncl, long nch);

int **i_matrix (long nrl, long nrh, long ncl, long nch);
void free_i_matrix (int **m, long nrl, long nrh, long ncl, long nch);

long **l_matrix (long nrl, long nrh, long ncl, long nch);
void free_l_matrix (long **m, long nrl, long nrh, long ncl, long nch);

#endif