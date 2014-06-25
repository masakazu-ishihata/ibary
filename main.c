#include "main.h"

int main(void)
{
  ul i;

  /* 64 bits */
  ibary *a = ibary_new(64);
  ibary *b = ibary_new(64);

  /* set : 4 bits * 8 = 32 bits */
  ibary_set_num(a, 0x1111111111111111);
  ibary_set_num(b, 0xffffffffffffffff);

  /* show */
  printf("a = %lx\n", 0x1111111111111111);
  ibary_show(stdout, a);
  printf("b = %lx\n", 0xffffffffffffffff);
  ibary_show(stdout, b);

  /* rank */
  printf("rank,   (a,0), (a,1), (b,0), (b,1)\n");
  for(i=0; i<50; i++){
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
  for(i=1; i<=16*2; i++){
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
  printf("Hamming(a, b) = %d\n", ibary_hamming(a, b));
  printf("Cosine(a, b)  = %e\n", ibary_cosine(a, b));

  /* free */
  ibary_free(a);
  ibary_free(b);

  return 0;
}
