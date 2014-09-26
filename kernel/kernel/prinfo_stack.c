#include <linux/prinfo_stack.h>
#include <linux/list.h>
#include <linux/slab.h>

static int s_count = 0;
struct list_head head;
struct list_head *head_ptr = NULL;

int s_init(void)
{
	printk("\nIn s_init: initializing stack pointer\n");
        if (head_ptr == NULL) {
                INIT_LIST_HEAD(&head);
                head_ptr = &head;
        }
	
        return 0;
}

int s_push(struct task_struct *task)
{
	struct stack_node *node = NULL;
	printk("\nIn s_push:\n");
        if (task == NULL)
                return -1;

	if (head_ptr == NULL) {
		s_init();
	}
	
	printk("Current count:%d\n", s_count);
        node = (struct stack_node *) kmalloc(sizeof(struct stack_node), GFP_KERNEL);
        node->task_ptr = task;
        list_add(&node->list, head_ptr);
        head_ptr = &(node->list);
        s_count++;
	printk("Successfully pushed, pid: %d,  final count:%d\n", task->pid, s_count);
        return 0;
}

struct task_struct *s_pop(void)
{
	struct stack_node *node = NULL;
	struct task_struct *task_ptr = NULL;
        printk("\nIn s_pop:\n");
        if (s_count == 0) {
                printk("Current count is %d\n", s_count);
                if (head_ptr != &head)
                        printk("but head_ptr doesnt point to head!!!!");
		printk("No popping\n");
                return NULL;
        }

	printk("Current count is %d\n", s_count);
        node = list_entry(head_ptr, struct stack_node, list);
	task_ptr = node->task_ptr;

	head_ptr = (node->list).prev;
	list_del(&node->list);
	kfree(node);
	s_count--;
	printk("Successfully popped, pid: %d, Final count is %d\n", task_ptr->pid, s_count);

	return task_ptr;
}

int is_stack_empty(void)
{
	return (s_count == 0);
}
