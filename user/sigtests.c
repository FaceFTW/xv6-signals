#include "kernel/signal.h"
#include "kernel/types.h"
#include "user/user.h"

void dummy_function() { printf("dummy function\n"); }

void example_handler() {
  printf("example handler\n");
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
    printf("parent now exiting");
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
    setsig(SIGMATH, example_handler);
    sleep(10);
    printf("doing unsafe math operations\n ");
    div(1, 0);
    printf("Child has not died because of custom handler!\n");
    exit(0);
  } else {
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
  int parent = getpid();
  int pid = fork();
  if (pid == 0) {
    printf("child sending sigusr to parent\n");
    sendsig(SIGUSR, parent);
    sleep(5);
    printf("child exiting\n");
    exit(0);
  } else {
    setsig(SIGUSR, example_handler);
    sleep(2);
    printf("parent waiting for child\n");
    wait(0);
    printf("parent now exiting");
  }
}

int main(int argc, char *argv[]) {
  //   sigkill_default_handler();
  //   sigkill_custom_ignored();
  //   sigusr_default_handler();
  //   sigusr_custom_handler();
  //   sigchld_default_handler();
  //   sigchld_custom_handler();

  //   sigmath_default_handler();
  sigmath_custom_handler();
  exit(0);
}
