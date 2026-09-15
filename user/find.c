#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

char *exec_argv[MAXARG];
int exec_argc = 0;

void
run_exec(char *matched_path)
{
  int i;
  char *argv[MAXARG];

  for (i = 0; i < exec_argc; i++) {
    argv[i] = exec_argv[i];
  }
  argv[i] = matched_path;
  argv[i + 1] = 0;

  int pid = fork();
  if (pid == 0) {
    exec(argv[0], argv);
    fprintf(2, "find: exec %s failed\n", argv[0]);
    exit(1);
  } else {
    wait(0);
  }
}

void
find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    if (strcmp(de.name, target) == 0) {
      if (exec_argc > 0) {
        run_exec(buf);
      } else {
        printf("%s\n", buf);
      }
    }

    if (st.type == T_DIR) {
      find(buf, target);
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "Usage: find directory filename [-exec cmd ...]\n");
    exit(1);
  }

  if (argc > 3) {
    if (strcmp(argv[3], "-exec") != 0) {
      fprintf(2, "Usage: find directory filename [-exec cmd ...]\n");
      exit(1);
    }
    for (int i = 4; i < argc; i++) {
      exec_argv[exec_argc++] = argv[i];
    }
  }

  find(argv[1], argv[2]);
  exit(0);
}
