#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	int mainPid = getpid();
		
	char command[25];
    sprintf(command, "pstree -p %d", getpid()); //wpisanie w stringa

	fork();
	
	printf("main process PID %d PPID %d\n", mainPid, getppid());
	
	int id;
	for (int i = 0; i < 1; i++) {
		id = fork();
		fork();
		if (id == 0) {
			fork();
			printf("proces potomnym PID: %d PPID %d UID %d GID %d\n", getpid(), getppid(), getuid(), getgid());
		}
	}
	
	if (id == 0) {
		sleep(1);
	} else {
		if (getpid() == mainPid) { //upewnienie sie że poniższe polecenia sa wykonywane przez main process
			fork();
			system(command);
			printf("\n");
		}
	}

	sleep(1);
	return 0;
}
