#ifndef _ISARY_H_
#define _ISARY_H_

#include <stdio.h>
#include <stdlib.h>

/*------------------------------------*/
/* define */
/*------------------------------------*/
typedef unsigned char uc;

/*------------------------------------*/
/* pop count */
/*------------------------------------*/
void ibary_init_poptable(void);
uc   ibary_popcount(int x);

/*------------------------------------*/
/* Binary Array */
/*------------------------------------*/
typedef struct IBARY
{
  size_t n; /* block size */
  uc *a;    /* array */
  uc *b;    /* block */
} ibary;

/* new */
ibary *ibary_new(size_t _n);
void   ibary_free(ibary *_b);

/* accessor */
void ibary_set(ibary *_b, int _i, int _v);
int  ibary_get(ibary *_b, int _i);

/* rank / select */
int ibary_rank(ibary *_b, int _v, int _i);
int ibary_select(ibary *_b, int _v, int _i);

/* show */
void ibary_show(FILE *_fp, ibary *_b);
void ibary_bit2str(uc _b, char *_c);

#endif
