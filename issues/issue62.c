#include <stdio.h>
struct s {
  struct {
    int i;
    int j;
  } t;
  int k;
};

int main() {
  struct s x = {.t.i = 1, .k = 2};
  //int x[2][3]={1,2,3,4,5,6,[0][1]=7,8,[0]=9,[0]=10,11,[1][0]=12}; 
  //printf("%d\n", x[0][2]);
  printf("%d %d %d\n", x.t.i, x.t.j, x.k);
    return 0;
}