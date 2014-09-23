#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include <linux/prinfo.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	/*struct prinfo temp;
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
	*/
	int size = 0;
	struct prinfo *kernel_buffer = NULL;

	if (buf == NULL || nr == NULL) {
		printk("prinfo: buf or nr is NULL\n");
		return -EINVAL;
	}
	/* TODO: Is access_ok required? */
	if (!access_ok(VERIFY_READ, (void *)nr, sizeof(int))) {
		printk("prinfo: nr is not a valid pointer\n");
		return -EFAULT;
	} else {
		if (copy_from_user(&size, nr, sizeof(int))) {
			printk("prinfo: copy_from_user failed to copy nr\n");
			return -EFAULT;
		}
	}
	if (size < 1) {
		printk("prinfo: no. of entries is less than 1\n");
		return -EINVAL;
	}
	if (!access_ok(VERIFY_READ, (void *)buf, sizeof(struct prinfo) * size)) {
		printk("prinfo: buf is not a valid buffer\n");
		return -EFAULT;
	} 

	kernel_buffer = (struct prinfo *)kmalloc((sizeof(struct prinfo) *
				size), GFP_KERNEL);
	if (!kernel_buffer) {
		printk("prinfo: couldn't allocate memory\n");
		return -ENOMEM;
	}
	
	//for debugging
	printk("prinfo: size = %d, buf = %x kernel_buffer = %x\n", size,
			(unsigned int)buf,
			(unsigned int)kernel_buffer);

	/* TODO: handle the condition when size is much larger than the actual no. of
	 * processes
	 */
	kfree(kernel_buffer);
	return 1;
}

