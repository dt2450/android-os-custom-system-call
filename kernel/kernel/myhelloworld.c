#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>

#include <linux/prinfo_stack.h>
#include <linux/sched.h>
#include <linux/slab.h>

SYSCALL_DEFINE0(myhelloworld)
{	
	printk (KERN_EMERG "HI FROM MYHELLOWORLD !!!");
	return 1;
}

void pop(int i)
{
	struct task_struct *n1;

	n1 = s_pop();
	if (n1 != NULL)
		printk("POP # %d returned task with pid: %d\n", i, n1->pid);
	else
		printk("POP # %d returned NULL\n", i);
}

SYSCALL_DEFINE3(ptree_1, char *, src, char *, dst, int, len)
{
	struct task_struct *n1, *n2, *n3;
	char buf;
	
	if (copy_from_user(&buf, src, len))
		return -EFAULT;
	
	if (copy_to_user(dst, &buf, len))
		return -EFAULT;


	n1 = current;
	n2 = (struct task_struct *) kmalloc(sizeof(struct task_struct), GFP_KERNEL);
	n2->pid = 666;

	n3 = (struct task_struct *) kmalloc(sizeof(struct task_struct), GFP_KERNEL);
        n3->pid = 2485;

	s_push(n1);
	s_push(n2);
	
	pop(1);
	s_push(n3);
	pop(2);
	pop(3);
	pop(4);
	pop(5);

	return len;	
}
