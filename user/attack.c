#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (100 * 4096)

int
main(int argc, char *argv[])
{
  char *data = sbrk(DATASIZE);
  char *prefix = "Here it is: ";
  int prefix_len = strlen(prefix);

  for (int i = 0; i < DATASIZE - prefix_len; i++) {
    int match = 1;
    for (int j = 0; j < prefix_len; j++) {
      if (data[i + j] != prefix[j]) {
        match = 0;
        break;
      }
    }
    if (match) {
      printf("%s\n", data + i + prefix_len);
      exit(0);
    }
  }

  printf("not found\n");
  exit(1);
}
