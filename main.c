#include "main.h"

#define BIT 64

int main(void)
{
  ul i, j;
  char str[256];

  /* 64 bits */
  ibary *a = ibary_new(BIT);
  ibary *b = ibary_new(BIT);

  /* a */
  ibary_set_num(a, 0x1111111111111111);
  ibary_string(a, str);
  printf("a = %s\n", str);
  ibary_show(stdout, a);

  /* b */
  ibary_set_num(b, 0xffffffffffffffff);
  ibary_string(b, str);
  printf("b = %s\n", str);
  ibary_show(stdout, b);

  /* count */
  printf("    v,    a,    b\n");
  for(i=0; i<2; i++)
    printf("%5lu,%5lu,%5lu\n", i, ibary_count(a, i), ibary_count(b, i));

  printf("    v,    u,  a-b\n");
  for(i=0; i<2; i++){
    for(j=0; j<2; j++)
      printf("%5lu,%5lu,%5lu\n", i, j, ibary_count2(a,b,i,j));
  }

  /* rank */
  printf("rank,   (a,0), (a,1), (b,0), (b,1)\n");
  for(i=0; i<BIT; i++){
    printf("%6lu,%6lu,%6lu,%6lu,%6lu\n",
           i,
           ibary_rank(a, 0, i),
           ibary_rank(a, 1, i),
           ibary_rank(b, 0, i),
           ibary_rank(b, 1, i)
           );
  }

  /* select */
  printf("select, (a,0), (a,1), (b,0), (b,1)\n");
  for(i=1; i<=BIT; i++){
    printf("%6lu,%6lu,%6lu,%6lu,%6lu\n",
           i,
           ibary_select(a, 0, i),
           ibary_select(a, 1, i),
           ibary_select(b, 0, i),
           ibary_select(b, 1, i)
           );
  }

  /* Jaccard index */
  printf("Jaccard(a, b) = %e\n", ibary_jaccard(a, b));
  printf("Hamming(a, b) = %lu\n", ibary_hamming(a, b));
  printf("Cosine(a, b)  = %e\n", ibary_cosine(a, b));

  /* free */
  ibary_free(a);
  ibary_free(b);

  return 0;
}
