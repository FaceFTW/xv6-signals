#include "kernel/signal.h"
#include "kernel/types.h"
#include "user/user.h"

void dummy_function() { printf("dummy function\n"); }

void example_handler() {
  printf("example handler\n");
  sigret();
}

int main(int argc, char *argv[]) {
  dummy_function();
  int pid = fork();
  if (pid == 0) {
    setsig(SIGUSR, example_handler);
    setsig(SIGMATH, dummy_function);
    while (1) {
      sleep(1);
      printf("child is alive yaaay\n");
    };
  } else {
    sleep(10);
    sendsig(SIGMATH, pid);
    sleep(5);
    sendsig(SIGUSR, pid);
    sleep(5);
    kill(pid);
  }

  exit(0);
}
