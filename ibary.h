#ifndef _IBARY_H_
#define _IBARY_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*------------------------------------*/
/* define */
/*------------------------------------*/
typedef unsigned char uc; /* 1 byte = 8 bit */
typedef unsigned int ui;  /* 2 byte = 16 bit */
typedef unsigned long ul; /* 4 byte = 32 bit */

/*------------------------------------*/
/* pop count */
/*------------------------------------*/
void ibary_init_poptable(void);
uc   ibary_popcount(ui x);

/*------------------------------------*/
/* Binary Array */
/*------------------------------------*/
typedef struct IBARY
{
  /* size */
  ul nb; /* # bits */
  ul ns; /* # small blocks */
  ul nl; /* # large blocks */

  /* arrays */
  ui *B; /* bit array */
  uc *S; /* small blocks */
  ul *L; /* large blocks */
} ibary;

/* new */
ibary *ibary_new(ul _n);
void   ibary_free(ibary *_b);

/* accessor */
uc   ibary_get(ibary *_b, ul _i);
void ibary_set(ibary *_b, ul _i, uc _v);
void ibary_set_num(ibary *_b, ul _n);

/* rank / select */
ul ibary_rank(ibary *_b, uc _v, ul _i);
ul ibary_select(ibary *_b, uc _v, ul _i);

/* rank / select with interval [s, t] */
ul ibary_irank(ibary *_b, uc _v, ul _s, ul _t);
ul ibary_iselect(ibary *_b, uc _v, ul _s, ul _t);

/* distance */
double ibary_jaccard(ibary *_a, ibary *_b);
double ibary_cosine(ibary *_a, ibary *_b);
int ibary_hamming(ibary *_a, ibary *_b);

/* show */
void ibary_show(FILE *_fp, ibary *_b);
void ibary_bit2str(ui _b, char *_c);

#endif
