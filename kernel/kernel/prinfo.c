#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

#include <linux/prinfo.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	struct prinfo temp;
	int len = (int) sizeof(struct prinfo);

	printk("prinfo: len=%d\n", len);

	if (copy_from_user(&temp, buf, len))
                return -EFAULT;
	
	printk("prinfo: initial temp.state=%lu\n", temp.state);
	temp.state = 666;
	if (copy_to_user(buf, &temp, len))
                return -EFAULT;
	
	printk("prinfo: final temp.state=%lu\n", temp.state);
	
	return 2485;

}

