#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

int main(int argc, char* argv[]) {
	int opid = getpid();
	int opar = getppid();
	printf("opid is: %d\n", opid);
	printf("opar is: %d\n", opar);
	int cpid;
	if((cpid = fork())) {
		printf("parent pid is: %d\n", getpid());
		printf("parent process group is: %d\n", getpgid(getpid()));
		printf("parent: %d\n", getpgid(0));
	}
	else {
		printf("child pid is: %d\n", getpid());
		printf("child process group is: %d\n", getpgid(getpid()));
		printf("child: %d\n", getpgid(0));
		setpgid(getpid(), 0);
		printf("child pid is now: %d\n", getpid());
		printf("child process group is now: %d\n", getpgid(getpid()));
		printf("child now: %d\n", getpgid(0));
	}
	return 0;
}
