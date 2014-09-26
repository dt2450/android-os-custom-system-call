#ifndef __LINUX_PRINFO_STACK_H
#define __LINUX_PRINFO_STACK_H

#include <linux/unistd.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/kernel.h>


struct stack_node
{
        struct task_struct *task_ptr;
        struct list_head list;
};


int s_push(struct task_struct *task);
struct task_struct *s_pop(void);
int is_stack_empty(void);

#endif
