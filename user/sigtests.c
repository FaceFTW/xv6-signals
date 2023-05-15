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
  //   void *p = malloc(100);

  //   printf("handler addr: %p\n", example_handler);

  int pid = fork();
  if (pid == 0) {
    setsig(SIGMATH, example_handler);
	sleep(5);
    setsig(SIGMATH, dummy_function);
    while (1) {
      sleep(1);
      printf("child is alive yaaay\n");
    };
  } else {
    sleep(20);
    sendsig(SIGMATH, pid);
  }

  //   sendsig(1, 2);
  //   setsig(1, &p);
  //   sigret();

  //   free(p);
  exit(0);
}
