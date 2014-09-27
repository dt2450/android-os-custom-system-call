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



int validate_input(int argc, char **argv)
{
	if (argc != 3) {
		printf ("Incorrect no. of args\n");
		printf ("usage: cmd <nr> <buf_sz>\n");
		return -1;
	}
	
	printf("argc=%d\n", argc);
	nr = atoi(argv[1]);
	buf_sz = atoi(argv[2]);
	printf("After atoi, nr=%d, buf_sz=%d\n", nr, buf_sz);
	if (nr < 0 || buf_sz < 0) {
		printf ("Invalid args\n");
                printf ("usage: cmd <nr> <buf_sz>\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int returnValue = validate_input(argc, argv);
	if(returnValue == -1){
		return returnValue;
	}
	int len = (int) sizeof(struct prinfo);
	printf("structure len = %d\n", len);

	struct prinfo *buf = NULL;
	if(buf_sz > 0){
		buf = (struct prinfo *) malloc(len * buf_sz);
	}

	int r = syscall(__NR_ptree, buf, &nr);
	
	if (r == -1) {
		printf("the error is : %s\n", strerror(errno));
		return -1;
	}
	
	printf(" Return value = %d\n", r);

	if(r>nr){
		printf("\n no of processes is larger than the buffer size!");
		printf("\n setting the max value of elements to max buffer size : %d\n",nr);
		r=nr;
	}

	int *p = malloc(sizeof(int) * r * 2);
	int i,j;
	*p = buf->pid;
	*(p+1) = 0;
	i=j=1;

	int k = i;

	while (i != ((2*r)-1) ) {
		int pid = *(p + i - 1);
		int noOfTabs = *(p + i);
		for(j=1; j < r; j++) {
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

	printf("\n\n\n\nProcess Tree is:\n");
	printf("=====================\n");
	printf("comm, pid, state, parentid, first child pid, next sibling pid, uid\n");
	for(i=0;i<r;i++){
		int currentPid = (buf+i)->pid;
		for(j=0;j<r*2;j+=2){
			if(currentPid == *(p+j)){
				int noOfTabs = *(p+j+1);
				int k;
				for(k=0;k<noOfTabs;k++){
					printf("\t");
				}
				printf("%d, %s,pid=%d,%ld,parent_pid=%d,f_child_pid=%d,%d,%ld\n",(i+1), (buf+i)->comm, (buf+i)->pid, (buf+i)->state,
					(buf+i)->parent_pid, (buf+i)->first_child_pid, (buf+i)->next_sibling_pid, (buf+i)->uid);
				break;
			}
		}
	}
	
			
	return 0;
}
