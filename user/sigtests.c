#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  sendsig(1, 2);

  exit(0);
}