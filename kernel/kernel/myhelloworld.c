#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(myhelloworld)
{	
	printk (KERN_EMERG "HI FROM MYHELLOWORLD !!!");
	return 1;
}
