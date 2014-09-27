#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <errno.h>


#define __NR_ptree	223

int nr, buf_sz;

struct prinfo {
        pid_t parent_pid;               /* process id of parent */
        pid_t pid;                      /* process id */
        pid_t first_child_pid;          /* pid of youngest child */
        pid_t next_sibling_pid;         /* pid of older sibling */
        long state;                     /* current state of process */
        long uid;                       /* user id of process owner */
        char comm[64];                  /* name of program executed */
};



void validate_input(int argc, char **argv)
{
	if (argc != 3) {
		printf ("Incorrect no. of args\n");
		printf ("usage: cmd <nr> <buf_sz>");
		exit(-1);
	}
	
	printf("argc=%d\n", argc);
	nr = atoi(argv[1]);
	buf_sz = atoi(argv[2]);
	printf("After atoi, nr=%d, buf_sz=%d\n", nr, buf_sz);
	if (nr < 0 || buf_sz < 0) {
		printf ("Invalid args\n");
                printf ("usage: cmd <nr> <buf_sz>");
		exit(-1);
	}
}

int main(int argc, char **argv)
{
	validate_input(argc, argv);

	int len = (int) sizeof(struct prinfo);
	printf("structure len = %d\n", len);

	struct prinfo *buf = (struct prinfo *) malloc(len * buf_sz);

	int r = syscall(__NR_ptree, buf, &nr);
	
	if (r == -1) {
		/*err no should be set and we can print error message*/
		printf("the error is : %s", strerror(errno));
		exit(-1);
	}
	
	int *p = malloc(sizeof(int) * len * 2);
	int i,j;
	*p = buf->pid;
	*(p+1) = 0;
	i=j=1;
	while (i != ((2*len)-1) ) {
		int pid = *(p + i - 1);
		int noOfTabs = *(p + i);
		int k = i;
		for(j=1; j < len; j++) {
			struct prinfo pinfo = *(buf+j);
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
		int currentPid = (buf+i)->pid;
		for(j=0;j<len*2;j+=2){
			if(currentPid == *(p+j)){
				int noOfTabs = *(p+j+1);
				int k;
				for(k=0;k<noOfTabs;k++){
					printf("\t");
				}
				printf("%s,%d,%ld,%d,%d,%d,%ld\n", (buf+i)->comm, (buf+i)->pid, (buf+i)->state,
					(buf+i)->parent_pid, (buf+i)->first_child_pid, (buf+i)->next_sibling_pid, (buf+i)->uid);
			}
		}
	}
	
			
        printf("Values are: ppid: %d pid: %d child_pid: %d sibling_pid: %d",
                        buf->parent_pid, buf->pid,
                        buf->first_child_pid,
                        buf->next_sibling_pid);
        printf(" state: %lu, uid: %lu, pname: %s\n", buf->state,
                        buf->uid, buf->comm);
	printf(" Return value = %d\n", r);
	return 0;
}
