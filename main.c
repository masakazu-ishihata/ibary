#include "main.h"

int main(void)
{
  int i;
  ibary *b = ibary_new(4); /* 4 byte (= 4 * 8 bit) */

  /* set */
  ibary_set(b,  0,  1);
  ibary_set(b,  5,  1);
  ibary_set(b, 10, 1);
  ibary_set(b, 15, 1);
  ibary_set(b, 20, 1);
  ibary_set(b, 25, 1);
  ibary_set(b, 30, 1);

  ibary_set(b, 25, 0);

  /* show */
  ibary_show(stdout, b);

  /* select */
  for(i=1; i<10; i++)
    printf("select(%d) = %2d, %2d\n", i, ibary_select(b, 1, i), ibary_select(b, 0, i));

  /* free */
  ibary_free(b);

  return 0;
}
