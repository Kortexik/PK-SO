#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

key_t key;
int shm_id, sem_id;
char *adres;

void crt_shm() {
    shm_id = shmget(key, sizeof(char), 0777 | IPC_CREAT);

    if (shm_id == -1) {
        perror("Problem z utworzeniem pamieci");
        exit(EXIT_FAILURE);
    } else {
        printf("Pamiec utworzona pod id: %d\n", shm_id);
    }
}

void att_shm() {
    adres = shmat(shm_id, (char *)0, 0);

    if (adres == (void *)-1) {
        perror("Problem z przydzieleniem adresu");
        exit(EXIT_FAILURE);
    } else {
        printf("Przestrzen adresowa zostala przyznana : %p\n", (void*)adres);
    }
}

void det_shm() {
    if (shmdt(adres) == -1) {
        perror("Problem z odlaczeniem pamieci dzielonej");
        exit(EXIT_FAILURE);
    }
    else printf("Udalo sie odloczyc pamiec\n");
}
static void semafor_p(int s_id, int s_n)
{
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num=s_n;
    bufor_sem.sem_op=-1;
    bufor_sem.sem_flg=0;
    zmien_sem=semop(sem_id,&bufor_sem,1);
    if (zmien_sem==-1)
      {
        if(errno == EINTR){
        semafor_p(s_id, s_n);
        }
        else
        {
        printf("Nie moglem zamknac semafora.\n");
        exit(EXIT_FAILURE);
        }
      }
    else
      {
        printf("Semafor zostal zamkniety.\n");
      }
}

static void semafor_v(int s_id, int s_n)
{
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num=s_n;
    bufor_sem.sem_op=1;
    bufor_sem.sem_flg=0;
    zmien_sem=semop(s_id,&bufor_sem,1);
    if (zmien_sem==-1)
      {
        printf("Nie moglem otworzyc semafora.\n");
        exit(EXIT_FAILURE);
      }
    else
      {
        printf("Semafor zostal otwarty.\n");
      }
}




void crt_sem() {
    sem_id = semget(key, 2, 0777 | IPC_CREAT);
    if (sem_id == -1) {
        perror("Problem przy tworzeniu semaforow");
        exit(EXIT_FAILURE);
    } else {
        printf("Semaforid: %d\n", sem_id);
    }
}

void clear_sem() {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Problem z usunieciem zbioru semaforow");
        exit(EXIT_FAILURE);
    }
    else printf("udalo sie usunac zbior semaforow\n");
}

int main() {
    key = ftok("./", 'h');
    printf("klucz = %d\n", key);

    FILE *pl;
    pl = fopen("wynik.txt", "w");
    if (pl == NULL) {
        perror("nie udalo sie otworrzyc pliku");
        exit(EXIT_FAILURE);
    } else {
        printf("Plik otwarty\n");
    }

    // Ustaw pamiec
    crt_shm();
    att_shm();
    crt_sem();

    while (1) {
        semafor_p(sem_id, 1); // wait for 1 p
        if (*adres == '\0') {
            break;
        }
        fputc(*adres, pl);
        printf("wpisano do pliku: %c\n", *adres);
        semafor_v(sem_id, 0); // signal 0 v
    }

    clear_sem();
    det_shm();
    shmctl(shm_id, IPC_RMID, 0); // Remove shared memory

    if(fclose(pl) != 0)
    {
        perror("Nie udalo sie zamknac pliku");
        exit(EXIT_FAILURE);
    }
    else printf("Plik zamkniety\n");

    exit(EXIT_SUCCESS);
}
