#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  void *p = malloc(100);
  sendsig(1, 2);
  setsig(1, &p);
  sigret();

  free(p);
  exit(0);
}