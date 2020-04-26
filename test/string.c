#include "test.h"

int main() {
  ASSERT(0, ""[0]);
  ASSERT(1, sizeof(""));

  ASSERT(97, "abc"[0]);
  ASSERT(98, "abc"[1]);
  ASSERT(99, "abc"[2]);
  ASSERT(0, "abc"[3]);
  ASSERT(4, sizeof("abc"));

  ASSERT(7, "\a"[0]);
  ASSERT(8, "\b"[0]);
  ASSERT(9, "\t"[0]);
  ASSERT(10, "\n"[0]);
  ASSERT(11, "\v"[0]);
  ASSERT(12, "\f"[0]);
  ASSERT(13, "\r"[0]);
  ASSERT(27, "\e"[0]);

  ASSERT(106, "\j"[0]);
  ASSERT(107, "\k"[0]);
  ASSERT(108, "\l"[0]);

  ASSERT(7, "\ax\ny"[0]);
  ASSERT(120, "\ax\ny"[1]);
  ASSERT(10, "\ax\ny"[2]);
  ASSERT(121, "\ax\ny"[3]);

  ASSERT(0, "\0"[0]);
  ASSERT(16, "\20"[0]);
  ASSERT(65, "\101"[0]);
  ASSERT(104, "\1500"[0]);
  ASSERT(0, "\x00"[0]);
  ASSERT(119, "\x77"[0]);

  ASSERT(7, sizeof("abc" "def"));
  ASSERT(9, sizeof("abc" "d" "efgh"));
  ASSERT(0, strcmp("abc" "d" "\nefgh", "abcd\nefgh"));
  ASSERT(0, !strcmp("abc" "d", "abcd\nefgh"));
  ASSERT(0, strcmp("\x9" "0", "\t0"));

  printf("OK\n");
  return 0;
}
