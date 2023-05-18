#include "kernel/signal.h"
#include "kernel/types.h"
#include "user/user.h"

static int test_global = -12;
int *test_ptr = &test_global;

void example_handler() {
  printf("example handler\n");
  sigret();
}

void example_handler_sigmath() {
  printf("uh oh, some bad math occured!!!!\n");
  sigret();
}

void example_handler_sigusr() {
  printf("modifying global user state\n");
  *test_ptr = 1;
  sigret();
}

// Example math function that can utilize SIGMATH
int div(int a, int b) {
  if (b == 0) {
    sendsig(SIGMATH, getpid());
  }

  return a / b;
}

void sigkill_default_handler() {
  int pid = fork();
  if (pid == 0) {
    while (1) {
      printf("child is alive\n");
      sleep(1);
    }
  } else {
    sleep(5);
    sendsig(SIGKILL, pid);
    printf("parent has sent sigkill to child\n");
  }
}

void sigmath_default_handler() {
  int pid = fork();
  if (pid == 0) {
    sleep(10);
    printf("doing unsafe math operations\n ");
    div(1, 0);
    printf("somehow this was not fatal!\n");
    exit(0);
  } else {
    printf("parent is not watching their child do bad things\n");
    wait(0);
    printf("parent has waited for child\n");
  }
}

void sigchld_default_handler() {
  int parent = getpid();
  int pid = fork();
  if (pid == 0) {
    printf("child is staying alive for a bit\n");
    sleep(10);
    sendsig(SIGCHLD, parent);
    exit(0);
  } else {
    sleep(5);
    wait(0);
    printf("parent has waited for child\n");
  }
}

void sigusr_default_handler() {
  int parent = getpid();
  int pid = fork();
  if (pid == 0) {
    printf("child sending sigusr to parent\n");
    sendsig(SIGUSR, parent);
    sleep(5);
    printf("child exiting\n");
    exit(0);
  } else {
    sleep(2);
    printf("parent waiting for child\n");
    wait(0);
    printf("parent now exiting\n");
  }
}

void sigkill_custom_ignored() {

  int pid = fork();
  if (pid == 0) {
    printf("child doesn't want to get murdered\n");
    setsig(SIGKILL, example_handler);
    while (1) {
      printf("child is alive\n");
      sleep(1);
    }
  } else {
    sleep(5);
    sendsig(SIGKILL, pid);
    printf("parent has still killed their child\n");
  }
}

void sigmath_custom_handler() {
  int pid = fork();
  if (pid == 0) {
    printf("child is preparing to do something risky...\n");
    setsig(SIGMATH, example_handler_sigmath);
    sleep(10);
    printf("doing unsafe math operations\n ");
    div(1, 0);
    printf("Child has not died because of custom handler!\n");
    exit(0);
  } else {
    sleep(5);
    printf("parent is not watching their child do bad things\n");
    wait(0);
    printf("parent has waited for child\n");
  }
}

void sigchld_custom_handler() {
  int parent = getpid();
  int pid = fork();
  if (pid == 0) {
    printf("child is staying alive for a bit\n");
    sleep(10);
    sendsig(SIGCHLD, parent);
    exit(0);
  } else {
    setsig(SIGCHLD, example_handler);
    sleep(5);
    wait(0);
    printf("parent has waited for child\n");
  }
}

void sigusr_custom_handler() {
  printf("global state is %d @ address: %p\n", test_global, &test_global);
  int parent = getpid();
  int pid = fork();
  if (pid == 0) {
    printf("child sending sigusr to parent\n");
    sleep(5);
    sendsig(SIGUSR, parent);
    sleep(5);
	printf("global state is %d @ address: %p\n", test_global, &test_global);

    printf("child exiting\n");
    exit(0);
  } else {
    setsig(SIGUSR, example_handler_sigusr);
    sleep(2);
    printf("parent waiting for child\n");
    wait(0);
    printf("parent now exiting\n");
  }
}

char *signames[] = {"SIGKILL", "SIGMATH", "SIGCHLD", "SIGUSR"};

struct test_fn {
  int signum;
  int custom_handler;
  void (*fn)();
};

struct test_fn tests[] = {
    {SIGKILL, 0, sigkill_default_handler}, {SIGKILL, 1, sigkill_custom_ignored},
    {SIGMATH, 0, sigmath_default_handler}, {SIGMATH, 1, sigmath_custom_handler},
    {SIGCHLD, 0, sigchld_default_handler}, {SIGCHLD, 1, sigchld_custom_handler},
    {SIGUSR, 0, sigusr_default_handler},   {SIGUSR, 1, sigusr_custom_handler},
};

void execute_test(struct test_fn *test) {
  printf("========================================\n");
  printf("Testing %s with %s handler\n", signames[test->signum],
         test->custom_handler ? "custom" : "default");
  printf("========================================\n");
  test->fn();
  sleep(5);
  printf("========================================\n");
  printf("Test complete\n");
  printf("========================================\n\n");
  if (test->custom_handler) {
    setsig(test->signum, (void (*)()) - 1);
  }
}

int main(int argc, char *argv[]) {

  printf("========================================\n");
  printf("Example Signal Handler Addresses\n");
  printf("========================================\n");
  printf("example_handler: %p\n", example_handler);
  printf("example_handler_sigmath: %p\n", example_handler_sigmath);
  printf("========================================\n\n");

  if (argc == 2) {
    int test_pid = fork();
    if (test_pid == 0) {
      execute_test(&tests[atoi(argv[1])]);
      exit(0);
    } else {
      wait(0);
    }
  } else {
    for (int i = 0; i < sizeof(tests) / sizeof(struct test_fn); i++) {
      int test_pid = fork();
      if (test_pid == 0) {
        execute_test(&tests[i]);
        exit(0);
      } else {
        wait(0);
      }
    }
  }
  exit(0);
}
