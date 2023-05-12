#include "kernel/signal.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  void *p = malloc(100);

  int pid = fork();
  if (pid == 0) {
    while (1) {
      sleep(1);
      printf("child is alive yaaay\n");
    };
  } else {
    sleep(10);
    sendsig(SIGKILL, pid);
  }

  //   sendsig(1, 2);
  setsig(1, &p);
  sigret();

  free(p);
  exit(0);
}