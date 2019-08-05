#!/bin/bash
cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }

int add6(int a, int b, int c, int d, int e, int f) {
  return a+b+c+d+e+f;
}
EOF

assert() {
  expected="$1"
  input="$2"

  ./chibicc "$input" > tmp.s
  gcc -static -o tmp tmp.s tmp2.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 'main() { return 5+20-4; }'
assert 41 'main() { return  12 + 34 - 5 ; }'
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 4 'main() { return (3+5)/2; }'
assert 10 'main() { return -10+20; }'
assert 10 'main() { return - -10; }'
assert 10 'main() { return - - +10; }'

assert 0 'main() { return 0==1; }'
assert 1 'main() { return 42==42; }'
assert 1 'main() { return 0!=1; }'
assert 0 'main() { return 42!=42; }'

assert 1 'main() { return 0<1; }'
assert 0 'main() { return 1<1; }'
assert 0 'main() { return 2<1; }'
assert 1 'main() { return 0<=1; }'
assert 1 'main() { return 1<=1; }'
assert 0 'main() { return 2<=1; }'

assert 1 'main() { return 1>0; }'
assert 0 'main() { return 1>1; }'
assert 0 'main() { return 1>2; }'
assert 1 'main() { return 1>=0; }'
assert 1 'main() { return 1>=1; }'
assert 0 'main() { return 1>=2; }'

assert 3 'main() { a=3; return a; }'
assert 8 'main() { a=3; z=5; return a+z; }'

assert 1 'main() { return 1; 2; 3; }'
assert 2 'main() { 1; return 2; 3; }'
assert 3 'main() { 1; 2; return 3; }'

assert 3 'main() { foo=3; return foo; }'
assert 8 'main() { foo123=3; bar=5; return foo123+bar; }'

assert 3 'main() { if (0) return 2; return 3; }'
assert 3 'main() { if (1-1) return 2; return 3; }'
assert 2 'main() { if (1) return 2; return 3; }'
assert 2 'main() { if (2-1) return 2; return 3; }'

assert 3 'main() { {1; {2;} return 3;} }'

assert 10 'main() { i=0; while(i<10) i=i+1; return i; }'
assert 55 'main() { i=0; j=0; while(i<=10) {j=i+j; i=i+1;} return j; }'

assert 55 'main() { i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'
assert 3 'main() { for (;;) return 3; return 5; }'

assert 3 'main() { return ret3(); }'
assert 5 'main() { return ret5(); }'
assert 8 'main() { return add(3, 5); }'
assert 2 'main() { return sub(5, 3); }'
assert 21 'main() { return add6(1,2,3,4,5,6); }'

assert 32 'main() { return ret32(); } ret32() { return 32; }'
assert 7 'main() { return add2(3,4); } add2(x,y) { return x+y; }'
assert 1 'main() { return sub2(4,3); } sub2(x,y) { return x-y; }'
assert 55 'main() { return fib(9); } fib(x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

assert 3 'main() { x=3; return *&x; }'
assert 3 'main() { x=3; y=&x; z=&y; return **z; }'
assert 5 'main() { x=3; y=5; return *(&x+1); }'
assert 3 'main() { x=3; y=5; return *(&y-1); }'
assert 5 'main() { x=3; y=&x; *y=5; return x; }'
assert 7 'main() { x=3; y=5; *(&x+1)=7; return y; }'
assert 7 'main() { x=3; y=5; *(&y-1)=7; return x; }'
assert 2 'main() { x=3; return (&x+2)-&x; }'

echo OK
