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
  _b->B = (ui *)calloc(_b->ns, sizeof(ui)); /* bit ary */
  _b->S = (uc *)calloc(_b->ns, sizeof(uc)); /* small blocks */
  _b->L = (ul *)calloc(_b->nl, sizeof(ul)); /* large blocks */

  return _b;
}
/*------------------------------------*/
/* clone */
/*------------------------------------*/
ibary *ibary_clone(ibary *_b)
{
  ibary *c = ibary_new(_b->nb);
  memmove(c->B, _b->B, _b->ns * sizeof(ui));
  memmove(c->S, _b->S, _b->ns * sizeof(uc));
  memmove(c->L, _b->L, _b->nl * sizeof(ul));
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


/*----------------------------------------------------------------------------*/
/* access / rank / select */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* access */
/*------------------------------------*/
uc ibary_access(ibary *_b, ul _i)
{
  int qs = _i / SBIT;   /* quotient  */
  int rs = _i % SBIT;   /* remainder */
  ui  m = mask_one(rs); /* bit mask  */
  return _b->B[qs] & m ? 1 : 0;
}
/*------------------------------------*/
/* rank(b, v, i) = # v's in b[0, i) */
/*------------------------------------*/
ul ibary_rank(ibary *_b, uc _v, ul _i)
{
  /* skip if i = 0 || i > _m->nb */
  if(_i == 0)     return 0;
  if(_i > _b->nb) return ibary_rank(_b, _v, _b->nb);

  /* rank */
  ul qs = (_i-1) / SBIT;
  ul ql = (_i-1) / LBIT;
  ul rs = (_i-1) % SBIT;
  ui m  = mask_all(rs);
  ui b  = ibary_poptable[ _b->B[qs] & m ] + _b->S[qs] + _b->L[ql];
  return _v ? b : min(_i, _b->nb) - b;
}
/*------------------------------------*/
/* select(b, v, i) = pos of the (i+1)-th v in b */
/*------------------------------------*/
ul ibary_select(ibary *_b, uc _v, ul _i)
{
  /* skip if invalid _i */
  if(_i+1 > ibary_rank(_b, _v, _b->nb)) return _b->nb;

  /* select */
  ul r, b;
  ul s = 0, t = _b->nb, m;

  /* binary search */
  do{
    m = (s + t) / 2;             /* mean */
    b = ibary_access(_b, m);     /* b[m-1] */
    r = ibary_rank(_b, _v, m+1); /* # _vs in b[s, t) */

    if(r == _i + 1 && b == _v) return m;
    else if(r < _i + 1) s = m + 1;
    else                t = m - 1;

    if(s > t){
      printf("error : %lu < %lu\n", s, t);
      exit(1);
    }
  }while(1);
}


/*----------------------------------------------------------------------------*/
/* irank / iselect */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/* irank(b, v, s, t) = # v's in b[s, t) */
/*------------------------------------*/
ul ibary_irank(ibary *_b, uc _v, ul _s, ul _t)
{
  return ibary_rank(_b, _v, _t) - ibary_rank(_b, _v, _s);
}
/*------------------------------------*/
/* iselect(b, v, s, i) = pos of the (i+1)-th v in b[_s:_m->nb) */
/*------------------------------------*/
ul ibary_iselect(ibary *_b, uc _v, ul _s, ul _i)
{
  return ibary_select(_b, _v, _i + ibary_rank(_b, _v, _s));
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
  return ibary_access(_b, _i);
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
/*------------------------------------*/
/* set num : b = _n */
/*------------------------------------*/
void ibary_set_num(ibary *_b, ul _n)
{
  ul i, q = _n;
  for(i=0, q=_n; i<_b->nb; i++, q=q/2)
    ibary_set(_b, i, q % 2);
}

/*----------------------------------------------------------------------------*/
/* counters */
/*----------------------------------------------------------------------------*/
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
  ui mf = mask_all(SBIT-1);

  for(i=0; i<_a->ns; i++){
    a = _v ? _a->B[i] : ~(_a->B[i]) & mf;
    b = _u ? _b->B[i] : ~(_b->B[i]) & mf;
    c = i < SBIT-1 ? a & b : a & b & ms;
    n += ibary_poptable[c];
  }

  return n;
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
  memset(_b->B, 0, _b->ns * sizeof(ui));
  memset(_b->S, 0, _b->ns * sizeof(uc));
  memset(_b->L, 0, _b->nl * sizeof(ul));
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
/* I : I[i] = select(_b, _v, i) */
/*------------------------------------*/
ul  *ibary_indices(ibary *_b, uc _v)
{
  ul i, j, n = ibary_count(_b, _v);
  ul *I = (ul *)malloc(n * sizeof(ul));

  /* sparse */
  if(n < _b->nb / log2(_b->nb))
    for(i=0; i<n; i++)
      I[i] = ibary_select(_b, _v, i);

  /* dence */
  else
    for(i=0, j=0; i<_b->nb; i++)
      if(ibary_access(_b, i) == _v)
        I[j++] = i;

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
/* string */
/* _str = (char *)_b[0,_n) */
/*------------------------------------*/
void ibary_string(ibary *_b, char *_str)
{
  ibary_substring(_b, 0, _b->nb, _str);
}
/*------------------------------------*/
/* substring */
/* _str = (char *)_b[_s, _t) */
/*------------------------------------*/
void ibary_substring(ibary *_b, ul _s, ul _t, char *_str)
{
  ul i;
  char *p;
  for(p=_str, i=0; i<=_t-_s; i++, p++)
    *p = ibary_access(_b, _t - i - 1) ? '1' : '0';
  *p = '\0';
}
/*------------------------------------*/
/* bits -> str */
/*------------------------------------*/
void ibary_bit2str(ui _b, char *_str)
{
  int i;
  char *p;
  ui m;
  for(p=_str, m=NUM/2, i=0; i<SBIT; i++, m=m>>1, p++)
    *p = _b & m ? '1' : '0';
  *p = '\0';
}
/*------------------------------------*/
/* show bit ary */
/*------------------------------------*/
void ibary_show(FILE *_fp, ibary *_b)
{
  int i, j, d;
  char c[16]; /* 10 bits + \0 */

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
