#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char separators[] = " -\r\t\n./,";

int
is_separator(char c)
{
  for (int i = 0; separators[i] != '\0'; i++) {
    if (c == separators[i])
      return 1;
  }
  return 0;
}

void
process(int fd)
{
  char c;
  int have_number = 0;
  int value = 0;
  int clean_start = 1;

  while (read(fd, &c, 1) == 1) {
    if (c >= '0' && c <= '9') {
      value = value * 10 + (c - '0');
      have_number = 1;
    } else {
      if (have_number) {
        int clean_end = is_separator(c);
        if (clean_start && clean_end && (value % 5 == 0 || value % 6 == 0))
          printf("%d\n", value);
      }
      have_number = 0;
      value = 0;
      clean_start = is_separator(c);
    }
  }

  if (have_number) {
    if (clean_start && (value % 5 == 0 || value % 6 == 0))
      printf("%d\n", value);
  }
}

int
main(int argc, char *argv[])
{
  int fd;

  if (argc <= 1) {
    process(0);
    exit(0);
  }

  for (int i = 1; i < argc; i++) {
    if ((fd = open(argv[i], 0)) < 0) {
      printf("sixfive: cannot open %s\n", argv[i]);
      continue;
    }
    process(fd);
    close(fd);
  }

  exit(0);
}
