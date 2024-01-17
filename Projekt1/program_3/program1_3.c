#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main() {
	printf("PID %d PPID %d\n", getpid(), getppid());

	int status;
	int id; 

	for (int i = 0; i < 3; i++) {
		id = fork();
		if (id == 0) {
			int executed = execl("./../I/a.out", NULL);
			if (executed == -1) {
				perror("Nie udalo sie wykonac komendy");
				exit(3);
			}
		} else if (id == -1) {
			perror("Nie udalo sie utworzyc podprocesu.");
			exit(1);
		}
	}

	if (id == 0) { 
                sleep(1);
        } else {
                char command[25];
                sprintf(command, "pstree -p %d", getpid());
                system(command);
                printf("\n");
        }

	for (int i = 0; i < 3; i++) {
		int pid = wait(&status);

		if (pid == -1) {
			perror("Nie udalo sie uzyskac podprocesu.");
			exit(2);
		} else {
			printf("ZAKONCZONO PROCES O PIDZIE %d - %d status \n", pid, status);
		}
	}

	sleep(1);
	return 0;
}
