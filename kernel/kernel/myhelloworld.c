#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(myhelloworld)
{	
	printk (KERN_EMERG "HI FROM MYHELLOWORLD !!!");
	return 1;
}

SYSCALL_DEFINE3(ptree_1, char *, src, char *, dst, int, len)
{
	char buf;
	
	if (copy_from_user(&buf, src, len))
		return -EFAULT;
	
	if (copy_to_user(dst, &buf, len))
		return -EFAULT;

	return len;	
}
