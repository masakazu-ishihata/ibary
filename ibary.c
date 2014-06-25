#include "ibary.h"

/*----------------------------------------------------------------------------*/
/* constants */
/*----------------------------------------------------------------------------*/
#define BIT 16
#define NUM 65536

/*----------------------------------------------------------------------------*/
/* macro function */
/*----------------------------------------------------------------------------*/
#define min(a, b) (a < b ? a : b)
#define ave(a, b) ( (a + b) / 2 )

/*----------------------------------------------------------------------------*/
/* pop table */
/*----------------------------------------------------------------------------*/
int  IBARY_POPTABLE_INITIALIZED = 0;
uc   ibary_poptable[NUM];
void ibary_init_poptable(void)
{
  ui i;
  if(!IBARY_POPTABLE_INITIALIZED){
    for(i=0; i<NUM; i++) ibary_poptable[i] = ibary_popcount(i);
    IBARY_POPTABLE_INITIALIZED = 1;
  }
}
uc ibary_popcount(ui x)
{
  uc n = 0;
  ui m = 0x01;
  for(; m!=0; m=m<<1) if(x & m) n++;
  return n;
}


/*----------------------------------------------------------------------------*/
/* bit array */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* _n bits ary */
/*------------------------------------*/
ibary *ibary_new(size_t _n)
{
  ibary *_b;
  int i;

  /* init poptable */
  ibary_init_poptable();

  /* binary array */
  _b = (ibary *)malloc(sizeof(ibary));
  _b->n = (_n + BIT -1) / BIT;
  _b->a = (ui *)malloc(_b->n * sizeof(ui));
  _b->b = (ul *)malloc(_b->n * sizeof(ul));

  for(i=0; i<_b->n; i++){
    _b->a[i] = 0;
    _b->b[i] = 0;
  }

  return _b;
}
/*------------------------------------*/
/* free */
/*------------------------------------*/
void ibary_free(ibary *_b)
{
  free(_b->a);
  free(_b->b);
  free(_b);
}


/*------------------------------------------------------------------------*/
/* accessor */
/*------------------------------------------------------------------------*/
/*------------------------------------*/
/* set : _b[_i] = _v */
/*------------------------------------*/
void ibary_set(ibary *_b, int _i, int _v)
{
  int q = _i / BIT;    /* quotient */
  int r = _i % BIT;    /* remainder */
  ui  m = 0x01 << r;   /* bit mask */
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
/*------------------------------------*/
/* set : _b = _n */
/*------------------------------------*/
void ibary_set_num(ibary *_b, ul _n)
{
  ui i, q;
  for(q=_n, i=0; i<BIT*_b->n; q=q/2, i++)
    ibary_set(_b, i, q % 2);
}
int ibary_get(ibary *_b, int _i)
{
  int q = _i / BIT;    /* quotient */
  int r = _i % BIT;    /* remainder */
  ui  m = 0x01 << r;   /* bit mask */

  if(_b->a[q] & m) return 1;
  else             return 0;
}

/*----------------------------------------------------------------------------*/
/* rank / select */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* rank(b, v, i) = # v's in b[0, i] */
/*------------------------------------*/
int ibary_rank(ibary *_b, int _v, int _i)
{
  int q, r, m, b;

  /* skip if invalid _i */
  if(_i < 0) return 0;

  /* total # _v in _b if _i > total length */
  if(_i >= BIT * _b->n)
    b = ibary_poptable[ _b->a[_b->n-1] ] + _b->b[ _b->n-1 ];

  /* # bits in b[0, _i] */
  else{
    q = _i / BIT; /* quotient  */
    r = _i % BIT; /* remainder */
    m = (0x01 << (r+1)) - 1; /* mask */
    b = ibary_poptable[ _b->a[q] & m ] + _b->b[q];
  }

  /* 0 / 1 */
  if(_v == 1)  return b;
  else         return _i + 1 - b;
}
/*------------------------------------*/
/* select(b, v, i) = pos of the i-th v of b */
/*------------------------------------*/
int ibary_select(ibary *_b, int _v, int _i)
{
  int r, b, i;
  int s=0, t=_b->n*BIT-1; /* interval */

  /* skip if invalid _i */
  if(_i < 0)  return -1;
  if(_i == 0) return 0;
  if(_i > ibary_rank(_b, _v, t)) return -1;

  /* binary search */
  do{
    i = ave(s, t);
    r = ibary_rank(_b, _v, i);
    b = ibary_get(_b, i);

    if(r == _i && b == _v) return i;
    else if(r < _i) s = i+1;
    else            t = i-1;
    if(s > t) exit(1);
  }while(1);
}


/*----------------------------------------------------------------------------*/
/* irank / iselect */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* irank(b, v, s, t) = # v's in b[s, t] */
/*------------------------------------*/
int ibary_irank(ibary *_b, int _v, int _s, int _t)
{
  return ibary_rank(_b, _v, _t) - ibary_rank(_b, _v, _s-1);
}
/*------------------------------------*/
/* iselect(b, v, s, i) = pos of the i-th v in b[_s, -1] */
/*------------------------------------*/
int ibary_iselect(ibary *_b, int _v, int _s, int _i)
{
  return ibary_select(_b, _v, _i + ibary_rank(_b, _v, _s-1));
}


/*----------------------------------------------------------------------------*/
/* distances */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* Jaccard index */
/* Jaccard(a, b) = |a & b| / |a v b| */
/*------------------------------------*/
double ibary_jaccard(ibary *_a, ibary *_b)
{
  int i, n = min(_a->n, _b->n);
  double de = 0, nu = 0;

  for(i=0; i<n; i++){
    nu += ibary_poptable[ _a->a[i] & _b->a[i] ];
    de += ibary_poptable[ _a->a[i] | _b->a[i] ];
  }
  return nu / de;
}
/*------------------------------------*/
/* cosine similarity */
/* cosine(a, b) = |a & b| / |a||b| */
/*------------------------------------*/
double ibary_cosine(ibary *_a, ibary *_b)
{
  int i, n = min(_a->n, _b->n);
  double de = 0, nu = 0;

  de = sqrt( ibary_rank(_a, 1, _a->n * 8) * ibary_rank(_b, 1, _b->n * 8) );
  for(i=0; i<n; i++)
    nu += ibary_poptable[ _a->a[i] | _b->a[i] ];

  return nu / de;
}
/*------------------------------------*/
/* Hamming distance */
/* Hamming(a, b) = |a, b|_1 (L1 norm) */
/*------------------------------------*/
int ibary_hamming(ibary *_a, ibary *_b)
{
  int i, h = 0, n = min(_a->n, _b->n);

  for(i=0; i<n; i++)
    h += ibary_poptable[ _a->a[i] ^ _b->a[i] ];

  return h;
}

/*----------------------------------------------------------------------------*/
/* show */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* show bit ary */
/*------------------------------------*/
void ibary_show(FILE *_fp, ibary *_b)
{
  int i, j, k;
  char c[10];

  /* index */
  fprintf(_fp, "index : ");
  for(i=_b->n-1; i>=0; i--){
    for(j=BIT-1; j>=0; j--){
      k = (BIT * i + j) % 10;
      fprintf(_fp, "%d", k);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");

  /* array */
  fprintf(_fp, "array : ");
  for(i=_b->n-1; i>=0; i--){
    ibary_bit2str(_b->a[i], c);
    fprintf(_fp, "%16s|", c);
  }
  fprintf(_fp, "\n");

  /* value */
  fprintf(_fp, "value : ");
  for(i=_b->n-1; i>=0; i--){
    fprintf(_fp, "%16d|", _b->a[i]);
  }
  fprintf(_fp, "\n");

  /* block */
  fprintf(_fp, "block : ");
  for(i=_b->n-1; i>=0; i--){
    fprintf(_fp, "%16ld|", _b->b[i]);
  }
  fprintf(_fp, "\n");

  /* rank 1 */
  fprintf(_fp, "rank1 : ");
  for(i=_b->n-1; i>=0; i--){
    for(j=BIT-1; j>=0; j--){
      k = i * BIT + j;
      fprintf(_fp, "%d", ibary_rank(_b, 1, k) % 10);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");

  /* rank 0 */
  fprintf(_fp, "rank0 : ");
  for(i=_b->n-1; i>=0; i--){
    for(j=BIT-1; j>=0; j--){
      k = i * BIT + j;
      fprintf(_fp, "%d", ibary_rank(_b, 0, k) % 10);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");
}
/*------------------------------------*/
/* bits -> str */
/*------------------------------------*/
void ibary_bit2str(ui _b, char *_c)
{
  int i;
  char *p;
  ui m;
  for(p=_c, m=NUM/2, i=0; i<BIT; i++, m=m>>1, p++){
    if(_b & m) *p = '1';
    else       *p = '0';
  }
  *p = '\0';
}
