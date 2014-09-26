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

	pid_t pid1 = fork();
	pid_t pid2 = fork();
	if(pid1 > 0) {
		printf("Parent pid = %d\n", getpid());
		if(pid2 == 0) {
			printf("Second Child pid = %d\n", getpid());
			sleep(1000);
			return 0;
		}
		pid_t pid3 = fork();
		if(pid3 == 0) {
			printf("Third Child pid = %d\n", getpid());
			sleep(5);
			int r = syscall(__NR_ptree, pp, &nr);

			printf(" Return value = %d\n", r);
			while (r > 0) {
				printf("Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d",
						pp->parent_pid, pp->pid,
						pp->first_child_pid,
						pp->next_sibling_pid);
				printf(" state: %lu, uid: %lu, pname: %s\n", pp->state,
						pp->uid, pp->comm);
				pp++;
				r--;
			}
			sleep(1000);
			return 0;
		}
	} else if(pid1 == 0) {
		if (pid2 > 0) {
			printf("First Child pid = %d\n", getpid());
			/*sleep(5);
			int r = syscall(__NR_ptree, pp, &nr);

			printf(" Return value = %d\n", r);
			while (r > 0) {
				printf("Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d",
						pp->parent_pid, pp->pid,
						pp->first_child_pid,
						pp->next_sibling_pid);
				printf(" state: %lu, uid: %lu, pname: %s\n", pp->state,
						pp->uid, pp->comm);
				pp++;
				r--;
			}*/
		} else if(pid2 == 0) {
			printf("Child Child pid = %d\n", getpid());
		}
		sleep(1000);
		return 0;
	}
	sleep(1000);
	return 0;
}
