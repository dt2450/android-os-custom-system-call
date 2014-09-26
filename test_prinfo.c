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

	int nr = 3;
	struct prinfo *pp = (struct prinfo *) malloc(len*nr);

	int r = syscall(__NR_ptree, pp, &nr);

        printf("Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d",
                        pp->parent_pid, pp->pid,
                        pp->first_child_pid,
                        pp->next_sibling_pid);
        printf(" state: %lu, uid: %lu, pname: %s\n", pp->state,
                        pp->uid, pp->comm);
	printf(" Return value = %d\n", r);
	sleep(10000);
	return 0;
}
