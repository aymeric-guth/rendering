#include <stdio.h>
#include <string.h>

int main() {
  const char *expr = "112+246*3";
  char *num_buf[2048] = {"\0"};
  char **expr_buffer[2048];

  for (int i = 0; i < strlen(expr); i++)
    printf("%c\n", *(expr + i));
}
