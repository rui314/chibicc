#include <stdio.h>

int main() {
    int m; 
#if defined(M13)
  m = 3;
#else
  m = 4;
#endif
printf("%d\n", m);
return 0;
}