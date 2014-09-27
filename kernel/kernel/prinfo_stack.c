#include <linux/prinfo_stack.h>
#include <linux/sched.h>
#include <linux/slab.h>

static int s_count = 0;
static int s_index = -1;
static int s_max_count = 0;
static struct task_struct **tasks = NULL;

int s_init(int max)
{
	//printk("\nIn s_init: initializing stack pointer\n");
	if (max <= 0) {
		pr_err("ptree: max value given as: %d\n", max);
		return -EFAULT;
	}

	s_pop_all();

	tasks = (struct task_struct **) kmalloc(sizeof(struct task_struct *) * max, GFP_KERNEL);
	if (tasks == NULL)
		return -ENOMEM;
	
	s_max_count = max;
        return 0;
}

int s_push(struct task_struct *task)
{
	printk("\nIn s_push:\n");
        if (tasks == NULL) {
		pr_err("tasks array is NULL\n");
                return -EFAULT;
	}

	if (s_count >= s_max_count) {
		pr_err("Overflow.\n");
		return -ENOMEM;
	}

	//printk("Current count:%d\n", s_count);
       // node = (struct stack_node *) kmalloc(sizeof(struct stack_node), GFP_KERNEL);
	s_index++;
	tasks[s_index] = task;
        s_count = s_index + 1;
	printk("Successfully pushed, pid: %d,  final count:%d\n", tasks[s_index]->pid, s_count);
        
	return 0;
}

struct task_struct *s_pop(void)
{
	struct task_struct *temp = NULL;
        //printk("\nIn s_pop:\n");
	if (tasks == NULL) {
		pr_err("tasks array is NULL\n");
		return NULL;
	}

        if (s_count == 0) {
		pr_err("Underflow.\n");
                return NULL;
        }

	//printk("Current count is %d\n", s_count);
	temp = tasks[s_index];
	s_index--;
	s_count = s_index + 1;
	printk("Successfully popped, pid: %d, Final count is %d\n", temp->pid, s_count);

	return temp;
}

int is_stack_empty(void)
{
	return (s_count == 0);
}

void s_pop_all(void)
{
	printk("\nIn s_pop_all:\n");
	if (tasks)
		kfree(tasks);
	tasks = NULL;
}
