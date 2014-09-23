#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#define __NR_ptree	223

struct prinfo {
        pid_t parent_pid;               /* process id of parent */
        pid_t pid;                      /* process id */
        pid_t first_child_pid;          /* pid of youngest child */
        pid_t next_sibling_pid;         /* pid of older sibling */
        long state;                     /* current state of process */
        long uid;                       /* user id of process owner */
        char comm[64];                  /* name of program executed */
};


int main()
{
	printf("Hi.\n");

	int len = (int) sizeof(struct prinfo);
	printf("len=%d\n", len);

	struct prinfo *pp = (struct prinfo *) malloc(len);
	pp->state = 111;
	int nr = 1;

	printf("initial buffer: %u.\n", (unsigned int)pp);
	printf("initial pp->state: %lu.\n", pp->state);
	int r = syscall(__NR_ptree, pp, &nr);
	printf("returned: %d.\n", r);
	printf("final pp->state: %lu.\n", pp->state);

	return 0;
}
