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
/* operators */
#define min(a, b) ( (a) < (b) ? (a) : (b) )
#define ave(a, b) ( ((a) + (b)) / 2 )

/* masks */
#define mask_one(n) ( (0x01 << (n)) )       /* 1 bit mask @ n */
#define mask_all(n) ( (0x01 << (n+1)) - 1 ) /* [0, n] bit mask */


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

  /* init poptable */
  ibary_init_poptable();

  /* init self */
  _b = (ibary *)malloc(sizeof(ibary));

  /* init size */
  _b->nb = _n;                      /* # bits */
  _b->ns = (_n + SBIT - 1) / SBIT;  /* # small blocks */
  _b->nl = (_n + LBIT - 1) / LBIT;  /* # large blocks */

  /* init arrays */
  _b->B = (ui *)malloc(_b->ns * sizeof(ui));
  _b->S = (uc *)malloc(_b->ns * sizeof(uc));
  _b->L = (ul *)malloc(_b->nl * sizeof(ul));

  /* clear */
  ibary_clear(_b);

  return _b;
}
/*------------------------------------*/
/* clone */
/*------------------------------------*/
ibary *ibary_clone(ibary *_b)
{
  ul i;
  ibary *c = ibary_new(_b->nb);
  for(i=0; i<c->ns; i++) c->B[i] = _b->B[i];
  for(i=0; i<c->ns; i++) c->S[i] = _b->S[i];
  for(i=0; i<c->nl; i++) c->L[i] = _b->L[i];
  return c;
}
/*------------------------------------*/
/* free */
/*------------------------------------*/
void ibary_free(void *_b)
{
  ibary *b = (ibary *)_b;
  free(b->B);
  free(b->L);
  free(b->S);
  free(b);
}


/*------------------------------------------------------------------------*/
/* accessor */
/*------------------------------------------------------------------------*/
/*------------------------------------*/
/* size : _b->nb */
/*------------------------------------*/
ul ibary_size(ibary *_b)
{
  return _b->nb;
}
/*------------------------------------*/
/* get : _b[i] */
/*------------------------------------*/
uc ibary_get(ibary *_b, ul _i)
{
  int qs = _i / SBIT;   /* quotient  */
  int rs = _i % SBIT;   /* remainder */
  ui  m = mask_one(rs); /* bit mask  */

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
  ul qs = _i / SBIT;    /* quotient */
  ul ql = _i / LBIT;
  ul rs = _i % SBIT;    /* remainder */
  ui m  = mask_one(rs); /* bit mask */
  ul i, n, b;

  if(ibary_get(_b, _i) != _v){
    /* set 0/1 */
    b = (_v == 1) ? 1 : -1;

    /* bit array */
    _b->B[qs] += b * m;

    /* large block */
    n = _b->nl;
    for(i=ql+1; i<n; i++) _b->L[i] += b;

    /* small block */
    n = min(SBIT * ((_i + LBIT) / LBIT), _b->ns);
    for(i=qs+1; i<n; i++) _b->S[i] += b;
  }
}


/*------------------------------------------------------------------------*/
/* operators */
/*------------------------------------------------------------------------*/
/*------------------------------------*/
/* and / or/ xor */
/*------------------------------------*/
ui and(ui _a, ui _b){ return _a & _b; }
ui or(ui _a, ui _b){  return _a | _b; }
ui xor(ui _a, ui _b){ return _a ^ _b; }
ui sub(ui _a, ui _b){ return (_a ^ (_a & _b)); }
void ibary_and(ibary *_a, ibary *_b){ ibary_operator(_a, _b, and); }
void ibary_or(ibary *_a, ibary *_b){  ibary_operator(_a, _b, or);  }
void ibary_xor(ibary *_a, ibary *_b){ ibary_operator(_a, _b, xor); }
void ibary_add(ibary *_a, ibary *_b){ ibary_operator(_a, _b, or); }
void ibary_sub(ibary *_a, ibary *_b){ ibary_operator(_a, _b, sub); }
/*------------------------------------*/
/* ibary_operator(a, b, *) : a = a * b */
/*------------------------------------*/
void ibary_operator(ibary *_a, ibary *_b, ui (*opt)(ui, ui))
{
  ul i, j, s, t;
  int d;

  for(i=0; i<_a->ns; i++){
    /* difference of counts */
    d = ibary_poptable[ opt(_a->B[i], _b->B[i]) ] - ibary_poptable[ _a->B[i] ];

    /* operate */
    _a->B[i] = opt(_a->B[i], _b->B[i]);

    if(d == 0) continue;

    /* large block */
    s = i / SBIT + 1;
    t = _a->nl;
    for(j=s; j<t; j++) _a->L[j] += d;

    /* small block */
    s = i + 1;
    t = min(SBIT * ((i + SBIT) / SBIT) ,_b->ns);
    for(j=s; j<t; j++) _a->S[j] += d;
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
  ui m  = mask_all(rs); /* rs bit mask */
  ui b;

  /* total # _v in _b if _i > total length */
  if(_i >= _b->nb) return ibary_rank(_b, _v, _b->nb-1);

  /* # bits in b[0, _i] */
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
  return ibary_count2(_a, _b, 1, 1) / (double)(_a->nb - ibary_count2(_a, _b, 0, 0));
}
/*------------------------------------*/
/* cosine similarity */
/* cosine(a, b) = |a & b| / |a||b| */
/*------------------------------------*/
double ibary_cosine(ibary *_a, ibary *_b)
{
  return ibary_count2(_a, _b, 1, 1) / sqrt( ibary_count(_a, 1) * ibary_count(_b, 1) );
}
/*------------------------------------*/
/* Hamming distance */
/* Hamming(a, b) = |a, b|_1 (L1 norm) */
/*------------------------------------*/
ul ibary_hamming(ibary *_a, ibary *_b)
{
  return ibary_count2(_a, _b, 1, 1) + ibary_count2(_a, _b, 0, 0);
}


/*----------------------------------------------------------------------------*/
/* utils */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* clear */
/*------------------------------------*/
void ibary_clear(ibary *_b)
{
  ul i;
  for(i=0; i<_b->ns; i++) _b->B[i] = 0;
  for(i=0; i<_b->ns; i++) _b->S[i] = 0;
  for(i=0; i<_b->nl; i++) _b->L[i] = 0;
}
/*------------------------------------*/
/* copy */
/*------------------------------------*/
void ibary_copy(ibary *_a, ibary *_b)
{
  ibary_clear(_a);
  ibary_or(_a, _b);
}
/*------------------------------------*/
/* count _v's in _b */
/*------------------------------------*/
ul   ibary_count(ibary *_b, uc _v)
{
  return ibary_rank(_b, _v, _b->nb);
}
/*------------------------------------*/
/* count2 */
/*------------------------------------*/
ul   ibary_count2(ibary *_a, ibary *_b, uc _v, uc _u)
{
  ui i, a, b, c;
  ul n  = 0;
  ul rs = _a->nb % SBIT;
  ui ms = mask_all(rs);
  ui mf = mask_all(SBIT);

  for(i=0; i<_a->ns; i++){
    a = _v ? _a->B[i] : ~(_a->B[i]) & mf;
    b = _u ? _b->B[i] : ~(_b->B[i]) & mf;
    c = i < SBIT-1 ? a & b : a & b & ms;
    n += ibary_poptable[c];
  }

  return n;
}
/*------------------------------------*/
/* I : I[i] = select(_b, _v, i+1) */
/*------------------------------------*/
ul  *ibary_indices(ibary *_b, uc _v)
{
  ul i, j, n = ibary_count(_b, _v);
  ul *I = (ul *)malloc(n * sizeof(ul));

  if(n < _b->nb / log2(_b->nb)){
    for(i=0; i<n; i++)
      I[i] = ibary_select(_b, _v, i+1);
  }else{
    for(i=0, j=0; i<_b->nb; i++)
      if(ibary_get(_b, i) == _v) I[j++] = i;
  }

  return I;
}
/*------------------------------------*/
/* return _a == _b */
/*------------------------------------*/
int  ibary_equal(ibary *_a, ibary *_b)
{
  ul i;

  /* different # of 1's */
  if(ibary_count(_a, 1) != ibary_count(_b, 1)) return 0;

  /* block */
  for(i=0; i<_a->ns; i++)
    if(_a->B[i] != _b->B[i]) return 0;

  return 1;
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
      fprintf(_fp, "%X", j);
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
