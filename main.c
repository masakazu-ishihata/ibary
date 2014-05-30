#include "main.h"

int main(void)
{
  int i;

  /* bit arys (4 bypte = 4 * 8 bit) */
  ibary *a = ibary_new(4);
  ibary *b = ibary_new(4);

  /* set */
  ibary_set_num(a, 0x11111111);
  ibary_set_num(b, 0xffffffff);

  /* show */
  printf("a = %x\n", 0x11111111);
  ibary_show(stdout, a);
  printf("b = %x\n", 0xffffffff);
  ibary_show(stdout, b);

  /* select */
  printf("select, (a,0), (a,1), (b,0), (b,1)\n");
  for(i=1; i<8*4; i++){
    printf("%6d,%6d,%6d,%6d,%6d\n",
           i,
           ibary_select(a, 0, i),
           ibary_select(a, 1, i),
           ibary_select(b, 0, i),
           ibary_select(b, 1, i)
           );
  }

  /* rank */
  printf("rank,   (a,0), (a,1), (b,0), (b,1)\n");
  for(i=0; i<50; i++){
    printf("%6d,%6d,%6d,%6d,%6d\n",
           i,
           ibary_rank(a, 0, i),
           ibary_rank(a, 1, i),
           ibary_rank(b, 0, i),
           ibary_rank(b, 1, i)
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
