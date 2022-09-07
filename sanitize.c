// from https://www.oreilly.com/library/view/secure-programming-cookbook/0596003943/ch01s01.html
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include <ctype.h>
   
extern char **environ;
   
/* These arrays are both NULL-terminated. */
static char *spc_restricted_environ[  ] = {
  "IFS= \t\n",
  "PATH=" _PATH_STDPATH,
  0
};
   
static char *spc_preserve_environ[  ] = {
  "TZ",
  0
};
   
void spc_sanitize_environment() {
  int    i;
  char   **new_environ, *ptr, *value, *var;
  size_t arr_size = 1, arr_ptr = 0, len, new_size = 0;
   
  for (i = 0;  (var = spc_restricted_environ[i]) != 0;  i++) {
    new_size += strlen(var) + 1;
    arr_size++;
  }
  for (i = 0;  (var = spc_preserve_environ[i]) != 0;  i++) {
    if (!(value = getenv(var))) continue;
    new_size += strlen(var) + strlen(value) + 2; /* include the '=' */
    arr_size++;
  }

  new_size += (arr_size * sizeof(char *));
  if (!(new_environ = (char **)malloc(new_size))) abort(  );
  new_environ[arr_size - 1] = 0;
   
  ptr = (char *)new_environ + (arr_size * sizeof(char *));
  for (i = 0;  (var = spc_restricted_environ[i]) != 0;  i++) {
    new_environ[arr_ptr++] = ptr;
    len = strlen(var);
    memcpy(ptr, var, len + 1);
    ptr += len + 1;
  }
  for (i = 0;  (var = spc_preserve_environ[i]) != 0;  i++) {
    if (!(value = getenv(var))) continue;
    new_environ[arr_ptr++] = ptr;
    len = strlen(var);
    memcpy(ptr, var, len);
    *(ptr + len + 1) = '=';
    memcpy(ptr + len + 2, value, strlen(value) + 1);
    ptr += len + strlen(value) + 2; /* include the '=' */
  }
   
  environ = new_environ;
}

//detect invalid characters like values like £$@
int validateArgs(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        int j = 0;
        while (argv[i][j] != '\x00') {
            char c = argv[i][j];
            if (!isalpha(c) && !isdigit(c) && c != '_' && c != '/' && c != '-' && c != '=' && c != ',' && c != '\'' && c != '"' && c != '.' && c!= '#' && c != '~') {
                printf("%s\n", argv[i]);
                return -1;
            }
            ++j;
        }
    }
    return 0;
}