#ifndef _IBARY_H_
#define _IBARY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
ibary *ibary_clone(ibary *_b);
void   ibary_free(void *_b);

/* access / rank / select */
uc ibary_access(ibary *_b, ul _i);
ul ibary_rank(ibary *_b, uc _v, ul _i);
ul ibary_select(ibary *_b, uc _v, ul _i);

/* rank / select with interval [s, t] */
ul ibary_irank(ibary *_b, uc _v, ul _s, ul _t);
ul ibary_iselect(ibary *_b, uc _v, ul _s, ul _t);

/* accessors */
ul   ibary_size(ibary *_b);
uc   ibary_get(ibary *_b, ul _i);
void ibary_set(ibary *_b, ul _i, uc _v);
void ibary_set_num(ibary *_b, ul _n);

/* counters */
ul   ibary_count(ibary *_b, uc _v);
ul   ibary_count2(ibary *_a, ibary *_b, uc _v, uc _u);

/* operators */
ui and(ui _a, ui _b);
ui or(ui _a, ui _b);
ui xor(ui _a, ui _b);
ui sub(ui _a, ui _b);
void ibary_add(ibary *_a, ibary *_b);
void ibary_sub(ibary *_a, ibary *_b);
void ibary_and(ibary *_a, ibary *_b);
void ibary_or(ibary *_a, ibary *_b);
void ibary_xor(ibary *_a, ibary *_b);
void ibary_operator(ibary *_a, ibary *_b, ui (*opt)(ui, ui));

/* distance */
double ibary_jaccard(ibary *_a, ibary *_b);
double ibary_cosine(ibary *_a, ibary *_b);
ul     ibary_hamming(ibary *_a, ibary *_b);

/* utils */
void ibary_clear(ibary *_b);
void ibary_copy(ibary *_a, ibary *_b);
ul  *ibary_indices(ibary *_b, uc _v);
int  ibary_equal(ibary *_a, ibary *_b);

/* string */
void ibary_string(ibary *_b, char *_str);
void ibary_substring(ibary *_b, ul _s, ul _t, char *_str);
void ibary_bit2str(ui _n, char *_str);

/* show */
void ibary_show(FILE *_fp, ibary *_b);

#endif
