#include "test.h"

char g3 = 3;
short g4 = 4;
int g5 = 5;
long g6 = 6;

int main() {
  ASSERT(1, ({ int x[3]={1,2,3}; x[0]; }));
  ASSERT(2, ({ int x[3]={1,2,3}; x[1]; }));
  ASSERT(3, ({ int x[3]={1,2,3}; x[2]; }));
  ASSERT(3, ({ int x[3]={1,2,3}; x[2]; }));

  ASSERT(2, ({ int x[2][3]={{1,2,3},{4,5,6}}; x[0][1]; }));
  ASSERT(4, ({ int x[2][3]={{1,2,3},{4,5,6}}; x[1][0]; }));
  ASSERT(6, ({ int x[2][3]={{1,2,3},{4,5,6}}; x[1][2]; }));

  ASSERT(0, ({ int x[3]={}; x[0]; }));
  ASSERT(0, ({ int x[3]={}; x[1]; }));
  ASSERT(0, ({ int x[3]={}; x[2]; }));

  ASSERT(2, ({ int x[2][3]={{1,2}}; x[0][1]; }));
  ASSERT(0, ({ int x[2][3]={{1,2}}; x[1][0]; }));
  ASSERT(0, ({ int x[2][3]={{1,2}}; x[1][2]; }));

  ASSERT('a', ({ char x[4]="abc"; x[0]; }));
  ASSERT('c', ({ char x[4]="abc"; x[2]; }));
  ASSERT(0, ({ char x[4]="abc"; x[3]; }));
  ASSERT('a', ({ char x[2][4]={"abc","def"}; x[0][0]; }));
  ASSERT(0, ({ char x[2][4]={"abc","def"}; x[0][3]; }));
  ASSERT('d', ({ char x[2][4]={"abc","def"}; x[1][0]; }));
  ASSERT('f', ({ char x[2][4]={"abc","def"}; x[1][2]; }));

  ASSERT(4, ({ int x[]={1,2,3,4}; x[3]; }));
  ASSERT(16, ({ int x[]={1,2,3,4}; sizeof(x); }));
  ASSERT(4, ({ char x[]="foo"; sizeof(x); }));

  ASSERT(4, ({ typedef char T[]; T x="foo"; T y="x"; sizeof(x); }));
  ASSERT(2, ({ typedef char T[]; T x="foo"; T y="x"; sizeof(y); }));
  ASSERT(2, ({ typedef char T[]; T x="x"; T y="foo"; sizeof(x); }));
  ASSERT(4, ({ typedef char T[]; T x="x"; T y="foo"; sizeof(y); }));

  ASSERT(1, ({ struct {int a; int b; int c;} x={1,2,3}; x.a; }));
  ASSERT(2, ({ struct {int a; int b; int c;} x={1,2,3}; x.b; }));
  ASSERT(3, ({ struct {int a; int b; int c;} x={1,2,3}; x.c; }));
  ASSERT(1, ({ struct {int a; int b; int c;} x={1}; x.a; }));
  ASSERT(0, ({ struct {int a; int b; int c;} x={1}; x.b; }));
  ASSERT(0, ({ struct {int a; int b; int c;} x={1}; x.c; }));

  ASSERT(1, ({ struct {int a; int b;} x[2]={{1,2},{3,4}}; x[0].a; }));
  ASSERT(2, ({ struct {int a; int b;} x[2]={{1,2},{3,4}}; x[0].b; }));
  ASSERT(3, ({ struct {int a; int b;} x[2]={{1,2},{3,4}}; x[1].a; }));
  ASSERT(4, ({ struct {int a; int b;} x[2]={{1,2},{3,4}}; x[1].b; }));

  ASSERT(0, ({ struct {int a; int b;} x[2]={{1,2}}; x[1].b; }));

  ASSERT(0, ({ struct {int a; int b;} x={}; x.a; }));
  ASSERT(0, ({ struct {int a; int b;} x={}; x.b; }));

  ASSERT(5, ({ typedef struct {int a,b,c,d,e,f;} T; T x={1,2,3,4,5,6}; T y; y=x; y.e; }));
  ASSERT(2, ({ typedef struct {int a,b;} T; T x={1,2}; T y, z; z=y=x; z.b; }));

  ASSERT(1, ({ typedef struct {int a,b;} T; T x={1,2}; T y=x; y.a; }));

  ASSERT(4, ({ union { int a; char b[4]; } x={0x01020304}; x.b[0]; }));
  ASSERT(3, ({ union { int a; char b[4]; } x={0x01020304}; x.b[1]; }));

  ASSERT(0x01020304, ({ union { struct { char a,b,c,d; } e; int f; } x={{4,3,2,1}}; x.f; }));

  ASSERT(3, g3);
  ASSERT(4, g4);
  ASSERT(5, g5);
  ASSERT(6, g6);

  printf("OK\n");
  return 0;
}
