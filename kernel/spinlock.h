#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

#endif // _KERNEL_SPINLOCK_H