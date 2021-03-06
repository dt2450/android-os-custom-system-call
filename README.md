# Operating Systems - Custom System Call in Android Kernel

# Authors:
- Devashi Tandon
- Lawrence Candes
- Pratyush Parimal

# Files:

- kernel/arch/arm/kernel/calls.S - syscall entry for 'ptree'.
- kernel/arch/arm/include/asm/unistd.h - defining the syscall number (223) for 'ptree'.

- kernel/include/linux/prinfo.h - declaration of 'prinfo' structure.
- kernel/include/linux/ptree_stack.h - interface for the stack operaations.
- kernel/kernel/ptree.c - implementation of the 'ptree' syscall.
- kernel/kernel/ptree_stack.c - implementation all the stack operations.
- kernel/kernel/Makefile - inclusion of the new C files added.

- prinfo.c - for testing the system call and printing the process tree with correct indentation.
- test_prinfo.c - some additional test-cases.
- Makefile - used for building the system call test code.

# System Test Call usage:

./prinfo <nr> <buf_size>

# Test Cases:

1. ./prinfo 1 0
output - Invalid argument (EINVAL)

2. ./prinfo 10 5
output - test process gets an abort signal while trying to perform a malloc just after system call returns

3. ./prinfo 6 10
output - test process prints the process tree with 6 processes in it  

4. ./prinfo 10 10
output - test process prints the process tree with 10  processes in it  

5. ./prinfo 100 100
otuput - test process prints the process tree with 58 processes in it - this happens since there are only 58 running 
	processes

Manual Test Cases:
1. 0x70000 address was passed as the buffer address to the system call and copy to user returned EFAULT
   - this implies that copy to user validates that the address is a user space address and not a kernel space address.
2. We created a hierarchy of processes using multiple forks and validated that the first child pid is the pid of the youngest,
   that is the largest pid of all the children, and the next sibling pid is the pid of the oldest sibling i.e. the least pid of 
   all the siblings 

# Problem 4

(a)
After running the program several times we find that the pid field in the prinfo structure changes the most. The other fields generally do not change.
The following are the fields in the prinfo struct and the reasons why they might change:
1. parent_pid - this will change only if the parent process that created the child changes. for example - if the parent dies the process may get re-parented and so the parent_pid changes
2. pid - this will change when the process is run again. This happens as there are many processes running and the kernel assigns the next available process id to the newly created process.
3. first_child_pid - pid of the most recent child created else set to 0, this will change only if the parent process keeps creating a new child process every now and then. This could be the case of the zygote process.
4. next_sibling_id - pid if the next sibling at the current level. This value is different for each process, since the has the id of the next sibling, but will not change over time unless a sibling was terminated and a new one created.
5. state - this indicates the state of process and should be 1 which is TASK_INTERRUPTIBLE, except swapper and currently running test process which have state 0 which translates to TASK_RUNNING . The other processes will show TASK_INTERRUPTIBLE since the test process is executing a system call.
6. uid - this is the id of the user/program that is running the program, unless we login with a different unix user this will not change.
7. comm - this will not cange as this is the name of the running program, if the program calls exec and replaces the binary then this field is changed.

(b)
After starting the mobile web-browser just one new process is started.
The parent process of android.browser is zygote. 
After using the home button to close the browser and re-running the test program no processes are removed from the process tree and we noticed that a kworker process is started, after a little searching we found that a kworker process is just a kernel worker thread.

When the home button is pressed android doesn't terminate the running process rather the contents of the screen buffer are changed and the kernel displays the android launcher screen.

(c)
 i)
The applications in Android require an instance of Dalvik VM (which is a PROCESS VM) to run. It is similar to how a java process VM is required by a normal java program in order to execute. The zygote process starts gets initialized at the very start and has linkages to all required libraries. The zygote then listens on a socket for commands from components which might want to create a new application. On receiving such a request, the zygote forks, and the new application gets an 'anready-initialized' instance of Dalvik VM to run. This results in speed-up.

ii)
The binary called 'zygote' doesn't exist.

The binary for this process is /system/bin/app_process. It's started as a service ininit.rc in the following way: 'service zygote /system/bin/app_process -Xzygote /system/bin --zygote --start-system-server'. So it starts with the name 'zygote'. As we know, when we call exe
c, we can give any name we want to the child process.


iii)
There could be following reasons why an embedded device would choose to use a process like zygote:
a) Such devices have less CPU power, and initializing a Dalvik VM for every applicatiom could be very resource-consuming. Forking an already-initialized VM would be a better solution.
b) The speedup is achieved by one more factor, which is to NOT copy the shared libraries by using zygote. In this case all core libraries can exist in one place as long as they're read-only.


# References:

1. http://isis.poly.edu/kulesh/stuff/src/klist/
2. http://stackoverflow.com/questions/9305992/linux-threads-and-process
3. http://coltf.blogspot.com/p/android-os-processes-and-zygote.html

# Notes

1. Although DFS doesnt specify the order in which siblings have to be printed,
   we are printing the younger sibling first.
2. If the process is the oldest sibling then its next sibling pid will be zero since it points to the parent.
