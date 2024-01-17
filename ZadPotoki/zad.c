#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    int pdesk[2];
    int fifodesk1, fifodesk2;
    time_t l_sekund, l_sekund2;
    struct tm *tm_czas;
    int rok, rok1;

    pipe(pdesk);

    time(&l_sekund);
    printf("Current time: %ld\n", l_sekund);

    if (fork() == 0) {
        mkfifo("kolejkaFIFO", 0666);

        if (fork() == 0) {
            printf("Enter the number of seconds:\n");
            scanf("%ld", &l_sekund);
            printf("l_sekund=%ld\n", l_sekund);

            fifodesk1 = open("kolejkaFIFO", O_WRONLY);
            write(fifodesk1, &l_sekund, sizeof(time_t));
        } else {
            fifodesk2 = open("kolejkaFIFO", O_RDONLY);
            read(fifodesk2, &l_sekund2, sizeof(l_sekund2));

            printf("Text read from FIFO: %ld\n", l_sekund2);

            tm_czas = gmtime(&l_sekund2);
            printf("Date: month=%d  day=%d  day of the week=%d  year=%d\n",
                   tm_czas->tm_mon + 1, tm_czas->tm_mday, tm_czas->tm_wday + 1, tm_czas->tm_year);

            rok1 = tm_czas->tm_year;
            write(pdesk[1], &rok1, sizeof(int));
        }
    } else {
        read(pdesk[0], &rok, sizeof(int));
        printf("Text from the pipe: %d\n", rok + 1900);
    }

    return 0;
}
