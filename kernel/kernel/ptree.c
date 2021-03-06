#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>

#include <linux/prinfo.h>
#include <linux/ptree_stack.h>

/*
* fn to process each taask_struct entry
* found while traversing the process-tree.
* The relevant fields are copied to the
* prinfo structure.
*/

void process_task(struct prinfo *output_struct,
		struct task_struct *input_struct)
{
	struct task_struct *p = input_struct;
	struct prinfo *out_prinfo = output_struct;
	struct task_struct *parent_ts = NULL;
	struct task_struct *temp = NULL;

	/* if the task_struct is for a thread, ignore it */
	if (!thread_group_leader(p))
		return;

	/*If real_parent is NULL, we set parent_pid=0.
	* We're considering real_parent as it always points
	* to the actual parent, even in case of threads.*/
	if (p->real_parent)
		out_prinfo->parent_pid = p->real_parent->pid;
	else
		out_prinfo->parent_pid = 0;
	out_prinfo->pid = p->pid;

	if (!list_empty(&p->children)) {
		/* take the most recent child which will be at the
		* tail of the list
		*/
		temp = list_entry(p->children.prev,
				struct task_struct, sibling);
		if (temp && (temp->pid != p->pid))
			out_prinfo->first_child_pid = temp->pid;
		else
			out_prinfo->first_child_pid = 0;
	} else
		out_prinfo->first_child_pid = 0;

	if (!list_empty(&p->sibling)) {

		/*First get the sibling task_truct*/
		temp = list_entry(p->sibling.prev, struct task_struct,
				children);

		/*Then get the parent task_struct using its pid*/
		parent_ts = p->real_parent;

		if (temp && temp != parent_ts) {
			/* list is not empty and next node doesn't point
			* to the children node of parent.
			* i.e there IS a sibling
			*/
			/* take the oldest sibling which
			* will be the first node in the list
			*/
			temp = list_entry(p->sibling.prev, struct task_struct,
					sibling);
			if (temp)
				out_prinfo->next_sibling_pid = temp->pid;
			else
				out_prinfo->next_sibling_pid = 0;
		} else
			out_prinfo->next_sibling_pid = 0;
	} else
		out_prinfo->next_sibling_pid = 0;

	/* get the state */
	out_prinfo->state = p->state;

	/* get user's id from cred field*/
	if (p->cred)
		out_prinfo->uid = p->cred->uid;
	else
		out_prinfo->uid = 0;
	/* the size of the array in task_struct is 16 bytes
	* so it will always fit in the the prinfo->comm array
	*/
	strncpy(out_prinfo->comm, p->comm, TASK_COMM_LEN);
	out_prinfo->comm[TASK_COMM_LEN] = '\0';
}

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	int size = 0;
	struct prinfo *kernel_buffer = NULL;
	struct prinfo *curr_prinfo = NULL;
	/*init_task is pointing to swapper not init*/
	struct task_struct *ts_ptr = NULL;
	struct list_head *list = NULL;
	struct task_struct *ts_child = NULL;
	int prinfo_count = 0;
	int process_count = 0;
	int ret_val = -1;

	if (buf == NULL || nr == NULL) {
		pr_err("ptree: buf or nr is NULL\n");
		return -EINVAL;
	}
	if (!access_ok(VERIFY_READ, (void *)nr,	sizeof(int))) {
		pr_err("ptree: nr is not a valid pointer\n");
		return -EFAULT;
	}
	if (copy_from_user(&size, nr, sizeof(int))) {
		pr_err("ptree: copy_from_user failed to copy nr\n");
		return -EFAULT;
	}

	if (size < 1) {
		pr_err("ptree: no. of entries is less than 1\n");
		return -EINVAL;
	}

	/*validating here since we can't wait for copy_to_user to detect
	it neaar the end*/
	if (!access_ok(VERIFY_WRITE, (void *)buf,
				sizeof(struct prinfo) * size)) {
		pr_err("ptree: buf is not a valid buffer\n");
		return -EFAULT;
	}

	kernel_buffer = kcalloc(size, sizeof(struct prinfo),
						GFP_KERNEL);
	if (!kernel_buffer)
		return -ENOMEM;

	curr_prinfo = kernel_buffer;

	/* initialize the stack with max no. of processes
	* that can be on the system
	*/
	ret_val = s_init(pid_max+1);
	if (ret_val) {
		pr_err("ptree: stack initialization failed.\n");
		kfree(kernel_buffer);
		return ret_val;
	}

	/*get the read lock*/
	read_lock(&tasklist_lock);

	/* start with swapper process which is the first process in Linux */
	ret_val = s_push(&init_task);
	if (ret_val) {
		pr_err("ptree: error in pushing to stack\n");
		read_unlock(&tasklist_lock);
		s_pop_all();
		kfree(kernel_buffer);
		return ret_val;
	}

	/* perform the DFS search in this loop */
	while (!is_stack_empty()) {
		ts_ptr = s_pop();
		if (ts_ptr == NULL) {
			pr_err("ptree: error in popping from stack\n");
			read_unlock(&tasklist_lock);
			s_pop_all();
			kfree(kernel_buffer);
			return -EFAULT;
		}
		if (prinfo_count < size) {
			/* handle the condition when size is larger
			* than the actual no. of processes
			*/
			process_task(curr_prinfo, ts_ptr);
			curr_prinfo++;
			prinfo_count++;
		}
		list = NULL;
		ts_child = NULL;
		list_for_each_prev(list, &ts_ptr->children) {
			ts_child = list_entry(list, struct task_struct,
					sibling);
			if (ts_child == NULL) {
				pr_err("ptree: error in traversing siblings");
				pr_err(" of process pid: %d\n", ts_ptr->pid);
				read_unlock(&tasklist_lock);
				s_pop_all();
				kfree(kernel_buffer);
				return -EFAULT;
			}
			ret_val = s_push(ts_child);
			if (ret_val) {
				pr_err("ptree: error in pushing to stack\n");
				read_unlock(&tasklist_lock);
				s_pop_all();
				kfree(kernel_buffer);
				return ret_val;
			}

		}
		process_count++;
	}

	/* release the lock */
	read_unlock(&tasklist_lock);

	if (copy_to_user(buf, kernel_buffer, (sizeof(struct prinfo) * size))) {
		pr_err("ptree: copy_to_user failed to copy kernel_buffer\n");
		kfree(kernel_buffer);
		return -EFAULT;
	}
	kfree(kernel_buffer);

	return process_count;
}
