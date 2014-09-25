#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>

#include <linux/prinfo.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	int size = 0;
	struct prinfo *kernel_buffer = NULL;
	struct prinfo *curr_prinfo = NULL;
	//init_task is pointing to swapper not init
	struct task_struct *p = &init_task;
	//struct task_struct *p = NULL;
	//for debugging
	int i = 0;
	struct task_struct *tp[4] = {&init_task, NULL, current, 0};
	struct task_struct *temp = NULL;

	if (buf == NULL || nr == NULL) {
		pr_err("prinfo: buf or nr is NULL\n");
		return -EINVAL;
	}
	tp[1] = find_task_by_pid_ns(1, &init_pid_ns);
	if (tp[1] == NULL) {
		pr_err("prinfo: couldn't find the task struct for init process\n");
		return -ESRCH;
	}
	/* TODO: Is access_ok required? */
	if (!access_ok(VERIFY_READ, (void *)nr, sizeof(int))) {
		pr_err("prinfo: nr is not a valid pointer\n");
		return -EFAULT;
	} else {
		if (copy_from_user(&size, nr, sizeof(int))) {
			pr_err("prinfo: copy_from_user failed to copy nr\n");
			return -EFAULT;
		}
	}
	if (size < 1) {
		pr_err("prinfo: no. of entries is less than 1\n");
		return -EINVAL;
	}
	if (!access_ok(VERIFY_WRITE, (void *)buf,
				sizeof(struct prinfo) * size)) {
		pr_err("prinfo: buf is not a valid buffer\n");
		return -EFAULT;
	}

	kernel_buffer = (struct prinfo *)kmalloc((sizeof(struct prinfo) *
				size), GFP_KERNEL);
	if (!kernel_buffer) {
		pr_err("prinfo: couldn't allocate memory\n");
		return -ENOMEM;
	}

	curr_prinfo = kernel_buffer;

	read_lock(&tasklist_lock);
	/* TODO: perform the DFS search in this block of code */
	do {
		p = tp[i];
		/* if the task_struct is for a thread, ignore it */
		if (!thread_group_leader(p))
			continue;
		if (p->real_parent)
			curr_prinfo->parent_pid = p->real_parent->pid;
		else
			curr_prinfo->parent_pid = 0;
		curr_prinfo->pid = p->pid;

		if (!list_empty(&p->children)) {
			temp = list_first_entry(&p->children,
					struct task_struct, sibling);
			if (temp && (temp->pid != p->pid)) {
				printk("Came here 1 temp->pid=%d\n", temp->pid);
				curr_prinfo->first_child_pid = temp->pid;
			} else {
				printk("Came here 2\n");
				curr_prinfo->first_child_pid = 0;
			}
		} else {
			printk("Came here 3\n");
			curr_prinfo->first_child_pid = 0;
		}
		if (!list_empty(&p->sibling)) {
			temp = list_first_entry(&p->sibling, struct task_struct,
					children);
			if (temp && temp !=
					find_task_by_pid_ns(p->real_parent->pid,
						&init_pid_ns)) {
				temp = list_first_entry(&p->sibling, struct task_struct,
						sibling);
				if (temp) {
					printk("Came here 4 temp->pid=%d\n", temp->pid);
					curr_prinfo->next_sibling_pid = temp->pid;
				} else {
					printk("Came here 5\n");
					curr_prinfo->next_sibling_pid = 0;
				}
			} else {
				printk("Came here 6\n");
				curr_prinfo->next_sibling_pid = 0;
			}
		} else {
			printk("Came here 7\n");
			curr_prinfo->next_sibling_pid = 0;
		}
		curr_prinfo->state = p->state;
		if (p->cred) {
			curr_prinfo->uid = p->cred->uid;
		} else
			curr_prinfo->uid = 0;
		/* the size of the array in task_struct is 16 bytes
		 * so it will always fit in the the prinfo->comm array
		 */
		strncpy(curr_prinfo->comm, p->comm, TASK_COMM_LEN);
		curr_prinfo->comm[TASK_COMM_LEN] = '\0';

	//for debugging
	pr_info("prinfo: size = %d, buf = %x kernel_buffer = %x\n", size,
			(unsigned int)buf,
			(unsigned int)kernel_buffer);
	pr_info("Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d",
			kernel_buffer->parent_pid, kernel_buffer->pid,
			kernel_buffer->first_child_pid,
			kernel_buffer->next_sibling_pid);
	pr_info(" state: %lu, uid: %lu, pname: %s\n",
			kernel_buffer->state,
			kernel_buffer->uid, kernel_buffer->comm);
	/*pr_info(
		 "curr_prinfo Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d\n",
			curr_prinfo->parent_pid, curr_prinfo->pid,
			curr_prinfo->first_child_pid,
			curr_prinfo->next_sibling_pid);
	*/
	} while (tp[++i] != 0 /* TODO: next_node_in_dfs */);
	read_unlock(&tasklist_lock);

	/* TODO: handle the condition when size is much larger than the actual no. of
	 * processes
	 */
	if (copy_to_user(buf, kernel_buffer, (sizeof(struct prinfo) * size))) {
		pr_err("prinfo: copy_to_user failed to copy kernel_buffer\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	kfree(kernel_buffer);
	return 1;
}

