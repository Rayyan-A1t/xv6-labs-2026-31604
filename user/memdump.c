#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  int offset = 0;

  for (int i = 0; fmt[i] != '\0'; i++) {
    char f = fmt[i];

    if (f == 'i') {
      if (offset + 4 > len) {
        printf("memdump: not enough data for '%c'\n", f);
        return;
      }
      int val;
      memmove(&val, data + offset, 4);
      printf("%d\n", val);
      offset += 4;

    } else if (f == 'p') {
      if (offset + 8 > len) {
        printf("memdump: not enough data for '%c'\n", f);
        return;
      }
      uint64 val;
      memmove(&val, data + offset, 8);
      printf("%lx\n", val);
      offset += 8;

    } else if (f == 'h') {
      if (offset + 2 > len) {
        printf("memdump: not enough data for '%c'\n", f);
        return;
      }
      short val;
      memmove(&val, data + offset, 2);
      printf("%d\n", val);
      offset += 2;

    } else if (f == 'c') {
      if (offset + 1 > len) {
        printf("memdump: not enough data for '%c'\n", f);
        return;
      }
      printf("%c\n", data[offset]);
      offset += 1;

    } else if (f == 's') {
      if (offset + 8 > len) {
        printf("memdump: not enough data for '%c'\n", f);
        return;
      }
      char *strptr;
      memmove(&strptr, data + offset, 8);
      printf("%s\n", strptr);
      offset += 8;

    } else if (f == 'S') {
      int j = offset;
      while (j < len && data[j] != '\0') {
        j++;
      }
      write(1, data + offset, j - offset);
      printf("\n");
      offset = len;
    }
  }
}
