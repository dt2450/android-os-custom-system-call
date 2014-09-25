#include <linux/prinfo_stack.h>

#include <linux/list.h>

static int s_count = 0;


int s_push(struct task_struct *task)
{
	return 0;
}

struct task_struct *s_pop(void)
{
	return NULL;
}
