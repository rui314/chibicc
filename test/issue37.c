int main() {
  int r = ({
    int x;
    goto label;
label:
    x;
  });
  return 0;
}