#include <linux/prinfo_stack.h>
#include <linux/sched.h>
#include <linux/slab.h>

static int s_count;
static int s_index = -1;
static int s_max_count;
static struct task_struct **tasks;

int s_init(int max)
{
	if (max <= 0) {
		pr_err("ptree: max value given as: %d\n", max);
		return -EFAULT;
	}

	s_pop_all();

	tasks = (struct task_struct **)
		kmalloc(sizeof(struct task_struct *) * max, GFP_KERNEL);
	if (tasks == NULL)
		return -ENOMEM;

	s_max_count = max;
	return 0;
}

int s_push(struct task_struct *task)
{
	if (tasks == NULL) {
		pr_err("tasks array is NULL\n");
		return -EFAULT;
	}

	if (s_count >= s_max_count) {
		pr_err("Stack overflow.\n");
		return -ENOMEM;
	}

	s_index++;
	tasks[s_index] = task;
	s_count = s_index + 1;

	return 0;
}

struct task_struct *s_pop(void)
{
	struct task_struct *temp = NULL;

	if (tasks == NULL) {
		pr_err("tasks array is NULL\n");
		return NULL;
	}

	if (s_count == 0) {
		pr_err("Stack underflow.\n");
		return NULL;
	}

	temp = tasks[s_index];
	s_index--;
	s_count = s_index + 1;

	return temp;
}

int is_stack_empty(void)
{
	return (s_count == 0);
}

void s_pop_all(void)
{
	kfree(tasks);
	tasks = NULL;
}
