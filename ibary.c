#include "ibary.h"

/*----------------------------------------------------------------------------*/
/* pop table */
/*----------------------------------------------------------------------------*/
int  IBARY_POPTABLE = 0;
uc   ibary_poptable[256];
void ibary_init_poptable(void)
{
  int i;
  if(IBARY_POPTABLE == 0){
    for(i=0; i<256; i++) ibary_poptable[i] = ibary_popcount(i);
    IBARY_POPTABLE = 1;
  }
}
uc ibary_popcount(int x)
{
  int n = 0, m = 0x01;
  for(; m!=0; m=m<<1) if(x & m) n++;
  return n;
}

/*----------------------------------------------------------------------------*/
/* bit array */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* new / free */
/*------------------------------------*/
ibary *ibary_new(size_t _n)
{
  ibary *_b;
  int i;

  /* init poptable */
  ibary_init_poptable();

  /* binary array */
  _b = (ibary *)malloc(sizeof(ibary));
  _b->n = _n;
  _b->a = (uc *)malloc(_n * sizeof(uc));
  _b->b = (uc *)malloc(_n * sizeof(uc));

  for(i=0; i<_n; i++){
    _b->a[i] = 0;
    _b->b[i] = 0;
  }

  return _b;
}
void ibary_free(ibary *_b)
{
  free(_b->a);
  free(_b->b);
  free(_b);
}

/*------------------------------------*/
/* accessor */
/*------------------------------------*/
void ibary_set(ibary *_b, int _i, int _v)
{
  int q = _i / 8;    /* quotient */
  int r = _i % 8;    /* remainder */
  uc  m = 0x01 << r; /* bit mask */
  int i;

  if(ibary_get(_b, _i) != _v){
    /* set 1 */
    if(_v == 1){
      _b->a[q] += m;
      for(i=q+1; i<_b->n; i++) _b->b[i] += 1;
    }
    /* set 0 */
    else if(_v == 0){
      _b->a[q] -= m;
      for(i=q+1; i<_b->n; i++) _b->b[i] -= 1;
    }
  }
}
int ibary_get(ibary *_b, int _i)
{
  int q = _i / 8;    /* quotient */
  int r = _i % 8;    /* remainder */
  uc  m = 0x01 << r; /* bit mask */

  if(_b->a[q] & m) return 1;
  else             return 0;
}

/*------------------------------------*/
/* rank / select */
/*------------------------------------*/
int ibary_rank(ibary *_b, int _v, int _i)
{
  int q = _i / 8; /* quotient  */
  int r = _i % 8; /* remainder */
  uc  m = (0x01 << (r+1)) - 1; /* mask */
  int b = ibary_poptable[ _b->a[q] & m] + _b->b[q];

  if(_v == 1)  return b;
  else         return _i + 1 - b;
}
int ibary_select(ibary *_b, int _v, int _i)
{
  int r, b, s=0, t=_b->n * 8, i;

  /* skip if invalid _i */
  if(_i == 0) return 0;
  if(_i > ibary_rank(_b, _v, 8 * _b->n - 1)) return -1;

  /* binary search */
  do{
    i = (s + t) / 2;
    r = ibary_rank(_b, _v, i);
    b = ibary_get(_b, i);
    if(r == _i && b == _v) return i;
    else if(r < _i) s = i+1;
    else            t = i-1;
  }while(1);
}

/*------------------------------------*/
/* show */
/*------------------------------------*/
void ibary_show(FILE *_fp, ibary *_b)
{
  int i, j, k;
  char c[10];

  /* index */
  fprintf(_fp, "index :");
  for(i=_b->n-1; i>=0; i--){
    for(j=7; j>=0; j--){
      k = (8 * i + j) % 10;
      fprintf(_fp, "%d", k);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");

  /* array */
  fprintf(_fp, "array :");
  for(i=_b->n-1; i>=0; i--){
    ibary_bit2str(_b->a[i], c);
    fprintf(_fp, "%8s|", c);
  }
  fprintf(_fp, "\n");

  /* block */
  fprintf(_fp, "block :");
  for(i=_b->n-1; i>=0; i--){
    fprintf(_fp, "%8d|", _b->b[i]);
  }
  fprintf(_fp, "\n");

  /* rank 1 */
  fprintf(_fp, "rank1 :");
  for(i=_b->n-1; i>=0; i--){
    for(j=7; j>=0; j--){
      k = i * 8 + j;
      fprintf(_fp, "%d", ibary_rank(_b, 1, k) % 10);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");

  /* rank 0 */
  fprintf(_fp, "rank0 :");
  for(i=_b->n-1; i>=0; i--){
    for(j=7; j>=0; j--){
      k = i * 8 + j;
      fprintf(_fp, "%d", ibary_rank(_b, 0, k) % 10);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");
}
/* bits -> str */
void ibary_bit2str(uc _b, char *_c)
{
  int i;
  char *p;
  uc m;
  for(p=_c, m=128, i=0; i<8; i++, m=m>>1, p++){
    if(_b & m) *p = '1';
    else       *p = '0';
  }
  *p = '\0';
}

