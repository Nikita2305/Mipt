#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main() {
  int fd = open("cur/c", O_RDONLY);
  char buffer[11];
  buffer[10] = '\0';
  read(fd, buffer, 10);
  printf("%s\n", buffer);

  read(fd, buffer, 10);
  printf("%s\n", buffer);
}
