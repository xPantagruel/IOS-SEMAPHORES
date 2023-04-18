/**
 * @file proj2.c
 * @author Matěj Macek (xmacek27)
 * @brief IOS project 2 - Post office
 * @date 2023-04-18
 */

#include <sys/stat.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

// file proj2.out
FILE *file;

// semaphores
sem_t *SEMNUMBER = NULL;
sem_t *SEMPRINT = NULL;
sem_t *CLOSEDOFFICECHECK = NULL;

// shared memory
int *NUMBER = NULL;
bool *CLOSEDOFFICE = NULL;

/**
 * @brief Initialize semaphores
 * 
 * @return true 
 * @return false 
 */
bool Initialize() {
    // semaphores
    SEMNUMBER = sem_open("/xmacek27.SEMNUMBER",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMNUMBER == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() failed SEMNUMBER\n");
        return false;
    }
    SEMPRINT = sem_open("/xmacek27.SEMPRINT",  O_CREAT | O_EXCL, 0666, 1);
    if (SEMPRINT == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    CLOSEDOFFICECHECK = sem_open("/xmacek27.CLOSEDOFFICE",  O_CREAT | O_EXCL, 0666, 0);
    if (CLOSEDOFFICECHECK == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    // shared memory
    if ((NUMBER = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed NUMBER\n");
        return false;
    }

    if ((CLOSEDOFFICE = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed CLOSEDOFFICE\n");
        return false;
    }

    return true;
}

/**
 * @brief Clean all resources
 * 
 */
void CleanAll() {
    // semaphores
    sem_close(SEMNUMBER);
    sem_unlink("/xmacek27.SEMNUMBER");

    sem_close(SEMPRINT);
    sem_unlink("/xmacek27.SEMPRINT");

    sem_close(CLOSEDOFFICECHECK);
    sem_unlink("/xmacek27.CLOSEDOFFICECHECK");

    // file
    fclose(file);

    // shared memory
    munmap(NUMBER, sizeof(int));
    munmap(CLOSEDOFFICE, sizeof(bool));
    
}

// 1 mutex . wait ()
// 2 if customers == n :
// 3 mutex . signal ()
// 4 balk ()
// 5 customers += 1
// 6 mutex . signal ()
// 7
// 8 customer . signal ()
// 9 barber . wait ()
// 10
// 11 # getHairCut ()
// 12
// 13 customerDone . signal ()
// 14 barberDone . wait ()
// 15
// 16 mutex . wait ()
// 17 customers -= 1
// 18 mutex . signal ()
void Customer(int id, int TZ) {
    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: Z %d: started\n",*NUMBER, id);
    sem_post(SEMPRINT);

    // usleep Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TZ>
    usleep(rand() % TZ);

    // case posta uzavrena: 
    sem_wait(CLOSEDOFFICECHECK);
        if(*CLOSEDOFFICE == true) 
        {
            sem_wait(SEMPRINT);
                *NUMBER += 1;
                fprintf(file, "%d: Z %d: going home\n",*NUMBER, id);
            sem_post(SEMPRINT);
            sem_post(CLOSEDOFFICECHECK);
            return;
        }
    sem_post(CLOSEDOFFICECHECK);

    // case posta otevrena:
    // vyberu nahodne cislo z intervalu <0,3>
    int X = rand() % 4;

    // vypise a zaradi se do fronty podle aktualniho cisla
    switch (X)
    {
    case 0:
        sem_wait(SEMPRINT);
            *NUMBER += 1;
            fprintf(file, "%d: Z %d: entering office a service %d\n",*NUMBER, id, X);
        sem_post(SEMPRINT);

        // todo zde bude semafor pro frontu
        // todo implementace zarazeni do fronty bude formou semaforu ktery bude cekat na signal od urednika a jakmile ho dostane vypise se called by office worker a zaradi se do fronty
        // todo kde bude cekat dokud ho neobslouzi urednik a pak  going home a proces konci 
        break;
    case 1:
        sem_wait(SEMPRINT);
            *NUMBER += 1;
                fprintf(file, "%d: Z %d: entering office a service %d\n",*NUMBER, id, X);
        sem_post(SEMPRINT);
        break;
    case 2:
        sem_wait(SEMPRINT);
            *NUMBER += 1;
            fprintf(file, "%d: Z %d: entering office a service %d\n",*NUMBER, id, X);
        sem_post(SEMPRINT);
        break;
    case 3:
        sem_wait(SEMPRINT);
            *NUMBER += 1;
            fprintf(file, "%d: Z %d: entering office a service %d\n",*NUMBER, id, X);
        sem_post(SEMPRINT);
        break;
    }



}

// 1 customer . wait ()
// 2 barber . signal ()
// 3
// 4 # cutHair ()
// 5
// 6 customerDone . wait ()
// 7 barberDone . signal ()
void Postman(int id) {
    sem_wait(SEMPRINT);
    fprintf(file, "%d\t: U %d\t: started\n", (*NUMBER), id);
    sem_post(SEMPRINT);
}

int main(int argc, char *argv[]) {
    char *endptr;

    // Check that there are 5 arguments
    if (argc != 6) {
        printf("Usage: %s NZ NU TZ TU F\n", argv[0]);
        exit(1);
    }

    // check that all arguments are valid integers
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j] != '\0'; j++) {
            if (!isdigit(argv[i][j])) {
                fprintf(stderr, "Invalid argument value\n");
                exit(1);
            }
        }
    }

    long NZ = strtol(argv[1], &endptr, 10);
    long NU = strtol(argv[2], &endptr, 10);
    long TZ = strtol(argv[3], &endptr, 10);
    long TU = strtol(argv[4], &endptr, 10);
    long F = strtol(argv[5], &endptr, 10);

    // Check for valid argument values
    if (NZ < 0 || NU < 0 || TZ < 0 || TU < 0 || F < 0) {
        fprintf(stderr, "Negative argument value\n");
        exit(1);
    }

    if (TZ > 10000 || TU > 100 || F > 10000) {
        fprintf(stderr, "Argument value too large\n");
        exit(1);
    }
	//opening file
	if ((file = fopen("proj2.out", "w")) == NULL)
	{
		return 1;
	}

	setbuf(file, NULL);

    // Initialize semaphores and shared memory
    if(!Initialize()){
        fprintf(stderr, "Error: semaphores initialization failed\n");
        CleanAll();
        exit(1);
    }

    (*NUMBER) = 0;

//------------------------------ Main Functionality ----------------------------------------------
// • NZ: počet zákazníků
// • NU: počet úředníků
// • TZ: Maximální čas v milisekundách, po který zákazník po svém vytvoření čeká, než vejde na poštu (eventuálně odchází s nepořízenou).
// • TU: Maximální délka přestávky úředníka v milisekundách.
// • F: Maximální čas v milisekundách, po kterém je uzavřena pošta pro nově příchozí. 

    // creating processes for customers
    pid_t pid = getpid();
	for (int i=0; i < NZ; i++)
	{
        pid = fork();
        if(pid == 0 ){
            Customer(i, TZ);
            exit(0);
        }
	}

    pid = getpid();
    // creating processes for postmen
	for (int i=0; i < NU; i++)
    {
        pid = fork();
        if (pid == 0) {
            Postman(i);
            exit(0);
        }
    }

    // wait for all processes to finish
    usleep((rand() % (F/2)) + F/2);

    // closing office
    sem_wait(CLOSEDOFFICECHECK);
        (*CLOSEDOFFICE) = true;
    sem_post(CLOSEDOFFICECHECK);

    // todo some waiting untill all processes finish

// -----------------------------------------------------------------------------------------------
	CleanAll();

    return 0;
}
