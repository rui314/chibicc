#include "test.h"

/*
 * This is a block comment.
 */

int main() {
  ASSERT(3, ({ int x; if (0) x=2; else x=3; x; }));
  ASSERT(3, ({ int x; if (1-1) x=2; else x=3; x; }));
  ASSERT(2, ({ int x; if (1) x=2; else x=3; x; }));
  ASSERT(2, ({ int x; if (2-1) x=2; else x=3; x; }));

  ASSERT(55, ({ int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; j; }));

  ASSERT(10, ({ int i=0; while(i<10) i=i+1; i; }));

  ASSERT(3, ({ 1; {2;} 3; }));
  ASSERT(5, ({ ;;; 5; }));

  ASSERT(10, ({ int i=0; while(i<10) i=i+1; i; }));
  ASSERT(55, ({ int i=0; int j=0; while(i<=10) {j=i+j; i=i+1;} j; }));

  ASSERT(3, (1,2,3));
  ASSERT(5, ({ int i=2, j=3; (i=5,j)=6; i; }));
  ASSERT(6, ({ int i=2, j=3; (i=5,j)=6; j; }));

  ASSERT(55, ({ int j=0; for (int i=0; i<=10; i=i+1) j=j+i; j; }));
  ASSERT(3, ({ int i=3; int j=0; for (int i=0; i<=10; i=i+1) j=j+i; i; }));

  ASSERT(1, 0||1);
  ASSERT(1, 0||(2-2)||5);
  ASSERT(0, 0||0);
  ASSERT(0, 0||(2-2));

  ASSERT(0, 0&&1);
  ASSERT(0, (2-2)&&5);
  ASSERT(1, 1&&5);

  printf("OK\n");
  return 0;
}
