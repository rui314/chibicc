// How to run:
//
// $ make
// $ ./chibicc examples/nqueen.c > tmp.s
// $ gcc -static -o tmp tmp.s
// $ ./tmp

int print_board(int (*board)[10]) {
  for (int i = 0; i < 10; i=i+1) {
    for (int j = 0; j < 10; j=j+1)
      if (board[i][j])
	printf("Q ");
      else
	printf(". ");
    printf("\n");
  }
  printf("\n\n");
}

int conflict(int (*board)[10], int row, int col) {
  for (int i = 0; i < row; i=i+1) {
    if (board[i][col])
      return 1;
    int j = row - i;
    if (0 < col - j + 1)
      if (board[i][col - j])
        return 1;
    if (col + j < 10)
      if (board[i][col + j])
        return 1;
  }
  return 0;
}

int solve(int (*board)[10], int row) {
  if (row > 9) {
    print_board(board);
    return 0;
  }
  for (int i = 0; i < 10; i=i+1) {
    if (conflict(board, row, i)) {
    } else {
      board[row][i] = 1;
      solve(board, row + 1);
      board[row][i] = 0;
    }
  }
}

int main() {
  int board[100];
  for (int i = 0; i < 100; i=i+1)
    board[i] = 0;
  solve(board, 0);
  return 0;
}
