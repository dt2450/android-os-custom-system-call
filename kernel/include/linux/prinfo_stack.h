#ifndef __LINUX_PRINFO_STACK_H
#define __LINUX_PRINFO_STACK_H

#include <linux/unistd.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/kernel.h>

int s_push(struct task_struct *task);
int s_init(int max);
struct task_struct *s_pop(void);
int is_stack_empty(void);
void s_pop_all(void);

#endif
