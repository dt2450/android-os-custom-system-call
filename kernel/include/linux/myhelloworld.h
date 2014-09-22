#ifndef __LINUX_MYHELLOWORLD_H
#define __LINUX_MYHELLOWORLD_H
	
#include <linux/unistd.h>
#include <linux/linkage.h>

_syscall0(int, myhelloworld);
_syscall3(int, ptree_1, char *, src, char *, dst, int, len);

#endif
