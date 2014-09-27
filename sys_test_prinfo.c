#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <errno.h>

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
	int nr = 1;

	int r = syscall(__NR_ptree, pp, &nr);
	
	if (r == -1) {
		/*err no should be set and we can print error message*/
		printf("the error is : %s",strerror(errno));
	}
	int *p = malloc(sizeof(int) * len * 2);
	int i,j;
	*p = pp->pid;
	*(p+1) = 0;
	i=j=1;
	while (i != ((2*len)-1) ) {
		int pid = *(p + i - 1);
		int noOfTabs = *(p + i);
		int k = i;
		for(j=1; j < len; j++) {
			struct prinfo pinfo = *(pp+j);
			if(pinfo.parent_pid == pid){
				k+=1;
				*(p+k) = pinfo.pid;
				k+=1;
				*(p+k) = noOfTabs+1;
			}
		}
		i+=2;	
	}
	printf("Process Tree is:\n");
	for(i=0;i<len;i++){
		int currentPid = (pp+i)->pid;
		for(j=0;j<len*2;j+=2){
			if(currentPid == *(p+j)){
				int noOfTabs = *(p+j+1);
				int k;
				for(k=0;k<noOfTabs;k++){
					printf("\t");
				}
				printf("%s,%d,%ld,%d,%d,%d,%ld\n", (pp+i)->comm, (pp+i)->pid, (pp+i)->state,
					(pp+i)->parent_pid, (pp+i)->first_child_pid, (pp+i)->next_sibling_pid, (pp+i)->uid);
			}
		}
	}
	
			
        printf("Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d",
                        pp->parent_pid, pp->pid,
                        pp->first_child_pid,
                        pp->next_sibling_pid);
        printf(" state: %lu, uid: %lu, pname: %s\n", pp->state,
                        pp->uid, pp->comm);
	printf(" Return value = %d\n", r);
	return 0;
}
