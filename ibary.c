#include "ibary.h"

/*----------------------------------------------------------------------------*/
/* constants */
/*----------------------------------------------------------------------------*/
#define SBIT 16
#define LBIT 256
#define NUM 65536

/*----------------------------------------------------------------------------*/
/* macro function */
/*----------------------------------------------------------------------------*/
#define min(a, b) ( (a) < (b) ? (a) : (b) )
#define ave(a, b) ( ((a) + (b)) / 2 )

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
ibary *ibary_new(ul _n)
{
  ibary *_b;
  int i;

  /* init poptable */
  ibary_init_poptable();

  _b = (ibary *)malloc(sizeof(ibary));

  /* size */
  _b->nb = _n;                      /* # bits */
  _b->ns = (_n + SBIT - 1) / SBIT;  /* # small blocks */
  _b->nl = (_n + LBIT - 1) / LBIT;  /* # large blocks */

  /* init arrays */
  _b->B = (ui *)malloc(_b->ns * sizeof(ui));
  _b->S = (uc *)malloc(_b->ns * sizeof(uc));
  _b->L = (ul *)malloc(_b->nl * sizeof(ul));

  /* init values */
  for(i=0; i<_b->ns; i++) _b->B[i] = 0;
  for(i=0; i<_b->ns; i++) _b->S[i] = 0;
  for(i=0; i<_b->nl; i++) _b->L[i] = 0;

  return _b;
}
/*------------------------------------*/
/* free */
/*------------------------------------*/
void ibary_free(ibary *_b)
{
  free(_b->B);
  free(_b->L);
  free(_b->S);
  free(_b);
}


/*------------------------------------------------------------------------*/
/* accessor */
/*------------------------------------------------------------------------*/
/*------------------------------------*/
/* get : _b[i] */
/*------------------------------------*/
uc ibary_get(ibary *_b, ul _i)
{
  int qs = _i / SBIT;   /* quotient  */
  int rs = _i % SBIT;   /* remainder */
  ui  m = 0x01 << rs;   /* bit mask  */

  if(_b->B[qs] & m) return 1;
  else              return 0;
}
/*------------------------------------*/
/* set : _b = _n */
/*------------------------------------*/
void ibary_set_num(ibary *_b, ul _n)
{
  ul q;
  ui i;
  for(q=_n, i=0; i<SBIT*_b->ns; q=q/2, i++)
    ibary_set(_b, i, q % 2);
}
/*------------------------------------*/
/* set : _b[_i] = _v */
/*------------------------------------*/
void ibary_set(ibary *_b, ul _i, uc _v)
{
  int qs = _i / SBIT; /* quotient */
  int ql = _i / LBIT;
  int rs = _i % SBIT; /* remainder */
  ui  m = 0x01 << rs; /* bit mask */
  int i, n, b;

  if(ibary_get(_b, _i) != _v){
    /* set 0/1 */
    b = (_v == 1) ? 1 : -1;

    /* bit array */
    _b->B[qs] += b * m;

    /* large block */
    n = _b->nl;
    for(i=ql+1; i<n; i++) _b->L[i] += b;

    /* small block */
    n = SBIT * ((_i + LBIT) / LBIT);
    n = min(n, _b->ns);
    for(i=qs+1; i<n; i++) _b->S[i] += b;
  }
}

/*----------------------------------------------------------------------------*/
/* rank / select */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* rank(b, v, i) = # v's in b[0, i] */
/*------------------------------------*/
ul ibary_rank(ibary *_b, uc _v, ul _i)
{
  ul qs = _i / SBIT;
  ul ql = _i / LBIT;
  ul rs = _i % SBIT;
  ui m = (0x01 << (rs + 1)) - 1;
  ui b;

  /* skip if invalid _i */
  if(_i < 0) return 0;

  /* total # _v in _b if _i > total length */
  if(_i >= _b->nb)
    b = ibary_poptable[ _b->B[_b->ns-1] ]
      + _b->S[ _b->ns-1 ]
      + _b->L[ _b->nl-1 ];

  /* # bits in b[0, _i] */
  else
    b = ibary_poptable[ _b->B[qs] & m ] + _b->S[qs] + _b->L[ql];

  /* 0 / 1 */
  return _v == 1 ? b : min(_i, _b->nb) + 1 - b;
}
/*------------------------------------*/
/* select(b, v, i) = pos of the i-th v of b */
/*------------------------------------*/
ul ibary_select(ibary *_b, uc _v, ul _i)
{
  ul r, b, i, s=0, t=_b->nb-1; /* interval */

  /* skip if invalid _i */
  if(_i < 0)  return _b->nb;
  if(_i == 0) return 0;
  if(_i > ibary_rank(_b, _v, t)) return _b->nb;

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
ul ibary_irank(ibary *_b, uc _v, ul _s, ul _t)
{
  return ibary_rank(_b, _v, _t) - ibary_rank(_b, _v, _s-1);
}
/*------------------------------------*/
/* iselect(b, v, s, i) = pos of the i-th v in b[_s, -1] */
/*------------------------------------*/
ul ibary_iselect(ibary *_b, uc _v, ul _s, ul _i)
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
  ul i, n = min(_a->ns, _b->ns);
  double de = 0, nu = 0;

  for(i=0; i<n; i++){
    nu += ibary_poptable[ _a->B[i] & _b->B[i] ];
    de += ibary_poptable[ _a->B[i] | _b->B[i] ];
  }
  return nu / de;
}
/*------------------------------------*/
/* cosine similarity */
/* cosine(a, b) = |a & b| / |a||b| */
/*------------------------------------*/
double ibary_cosine(ibary *_a, ibary *_b)
{
  ul i, n = min(_a->ns, _b->ns);
  double de = 0, nu = 0;

  de = sqrt( ibary_rank(_a, 1, _a->nb) * ibary_rank(_b, 1, _b->nb) );
  for(i=0; i<n; i++)
    nu += ibary_poptable[ _a->B[i] & _b->B[i] ];

  return nu / de;
}
/*------------------------------------*/
/* Hamming distance */
/* Hamming(a, b) = |a, b|_1 (L1 norm) */
/*------------------------------------*/
int ibary_hamming(ibary *_a, ibary *_b)
{
  ul i, h = 0, n = min(_a->ns, _b->ns);

  for(i=0; i<n; i++)
    h += ibary_poptable[ _a->B[i] ^ _b->B[i] ];

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
  int i, j, d;
  char c[10];

  printf("%lu, %lu, %lu\n", _b->nb, _b->ns, _b->nl);

  /* index */
  fprintf(_fp, "index : ");
  for(i=_b->ns-1; i>=0; i--){
    for(j=SBIT-1; j>=0; j--){
      fprintf(_fp, "%u", (SBIT * i + j) % 10);
    }
    fprintf(_fp, "|");
  }
  fprintf(_fp, "\n");

  /* array */
  fprintf(_fp, "array : ");
  for(i=_b->ns-1; i>=0; i--){
    ibary_bit2str(_b->B[i], c);
    fprintf(_fp, "%*s|", SBIT, c);
  }
  fprintf(_fp, "\n");

  /* value */
  fprintf(_fp, "value : ");
  for(i=_b->ns-1; i>=0; i--){
    fprintf(_fp, "%*u|", SBIT, _b->B[i]);
  }
  fprintf(_fp, "\n");

  /* large block */
  d = min(SBIT+LBIT-1, (SBIT+1)*_b->ns-1);
  fprintf(_fp, "large : ");
  for(i=_b->nl-1; i>=0; i--){
    fprintf(_fp, "%*lu|", d, _b->L[i]);
  }
  fprintf(_fp, "\n");

  /* small block */
  fprintf(_fp, "small : ");
  for(i=_b->ns-1; i>=0; i--){
    fprintf(_fp, "%*u|", SBIT, _b->S[i]);
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
  for(p=_c, m=NUM/2, i=0; i<SBIT; i++, m=m>>1, p++){
    if(_b & m) *p = '1';
    else       *p = '0';
  }
  *p = '\0';
}
