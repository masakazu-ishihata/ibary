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
  size_t n; /* # blocks */
  ui *a;    /* array */
  ul *b;    /* block */
} ibary;

/* new */
ibary *ibary_new(size_t _n);
void   ibary_free(ibary *_b);

/* accessor */
void ibary_set(ibary *_b, int _i, int _v);
void ibary_set_num(ibary *_b, ul _n);
int  ibary_get(ibary *_b, int _i);

/* rank / select */
int ibary_rank(ibary *_b, int _v, int _i);
int ibary_select(ibary *_b, int _v, int _i);

/* rank / select with interval [s, t] */
int ibary_irank(ibary *_b, int _v, int _s, int _t);
int ibary_iselect(ibary *_b, int _v, int _s, int _t);

/* distance */
double ibary_jaccard(ibary *_a, ibary *_b);
double ibary_cosine(ibary *_a, ibary *_b);
int ibary_hamming(ibary *_a, ibary *_b);

/* show */
void ibary_show(FILE *_fp, ibary *_b);
void ibary_bit2str(ui _b, char *_c);

#endif
