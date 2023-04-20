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
sem_t *SEMCUSTOMER = NULL;
sem_t *SEMPRINT = NULL;
sem_t *CLOSEDOFFICECHECK = NULL;
sem_t *SERVICE1 = NULL;
sem_t *SERVICE2 = NULL;
sem_t *SERVICE3 = NULL;
sem_t *SERVICE4 = NULL;
sem_t *CUSTOMERNUM1 = NULL;
sem_t *CUSTOMERNUM2 = NULL;
sem_t *CUSTOMERNUM3 = NULL;
sem_t *CUSTOMERNUM4 = NULL;

// shared memory
int *NUMBER = NULL;
bool *CLOSEDOFFICE = NULL;
int *CUSTOMER1 = NULL;
int *CUSTOMER2 = NULL;
int *CUSTOMER3 = NULL;
int *CUSTOMERS = NULL;

/**
 * @brief Initialize semaphores and shared memory
 * 
 * @return true 
 * @return false 
 */
bool Initialize() {
    // semaphores
    SEMCUSTOMER = sem_open("/xmacek27.SEMCUSTOMER",  O_CREAT | O_EXCL, 0666, 1);
    if (SEMCUSTOMER == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMCUSTOMER failed SEMCUSTOMER\n");
        return false;
    }
    SEMPRINT = sem_open("/xmacek27.SEMPRINT",  O_CREAT | O_EXCL, 0666, 1);
    if (SEMPRINT == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMPRINT failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    CLOSEDOFFICECHECK = sem_open("/xmacek27.CLOSEDOFFICECHECK",  O_CREAT | O_EXCL, 0666, 1);
    if (CLOSEDOFFICECHECK == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() CLOSEDOFFICE failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SERVICE1 = sem_open("/xmacek27.SERVICE1",  O_CREAT | O_EXCL, 0666, 0);
    if (SERVICE1 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SERVICE1 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SERVICE2 = sem_open("/xmacek27.SERVICE2",  O_CREAT | O_EXCL, 0666, 0);
    if (SERVICE2 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SERVICE2 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SERVICE3 = sem_open("/xmacek27.SERVICE3",  O_CREAT | O_EXCL, 0666, 0);
    if (SERVICE3 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SERVICE3 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SERVICE4 = sem_open("/xmacek27.SERVICE4",  O_CREAT | O_EXCL, 0666, 0);
    if (SERVICE4 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SERVICE4 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    CUSTOMERNUM1 = sem_open("/xmacek27.CUSTOMERNUM1",  O_CREAT | O_EXCL, 0666, 1);
    if (CUSTOMERNUM1 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() CUSTOMERNUM1 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    CUSTOMERNUM2 = sem_open("/xmacek27.CUSTOMERNUM2",  O_CREAT | O_EXCL, 0666, 1);
    if (CUSTOMERNUM2 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() CUSTOMERNUM2 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    CUSTOMERNUM3 = sem_open("/xmacek27.CUSTOMERNUM3",  O_CREAT | O_EXCL, 0666, 1);
    if (CUSTOMERNUM3 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() CUSTOMERNUM3 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    CUSTOMERNUM4 = sem_open("/xmacek27.CUSTOMERNUM4",  O_CREAT | O_EXCL, 0666, 1);
    if(CUSTOMERNUM4 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() CUSTOMERNUM4 failed with error code %d: %s\n", errno, strerror(errno));
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

    if ((CUSTOMER1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed NUMBER\n");
        return false;
    }

    if ((CUSTOMER2 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed NUMBER\n");
        return false;
    }

    if ((CUSTOMER3 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed NUMBER\n");
        return false;
    }

    if ((CUSTOMERS = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed NUMBER\n");
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
    sem_close(SEMCUSTOMER);
    sem_unlink("/xmacek27.SEMCUSTOMER");

    sem_close(SEMPRINT);
    sem_unlink("/xmacek27.SEMPRINT");

    sem_close(CLOSEDOFFICECHECK);
    sem_unlink("/xmacek27.CLOSEDOFFICECHECK");

    sem_close(SERVICE1);
    sem_unlink("/xmacek27.SERVICE1");

    sem_close(SERVICE2);
    sem_unlink("/xmacek27.SERVICE2");

    sem_close(SERVICE3);
    sem_unlink("/xmacek27.SERVICE3");

    sem_close(SERVICE4);
    sem_unlink("/xmacek27.SERVICE4");

    sem_close(CUSTOMERNUM1);
    sem_unlink("/xmacek27.CUSTOMERNUM1");

    sem_close(CUSTOMERNUM2);
    sem_unlink("/xmacek27.CUSTOMERNUM2");

    sem_close(CUSTOMERNUM3);
    sem_unlink("/xmacek27.CUSTOMERNUM3");
    
    sem_close(CUSTOMERNUM4);
    sem_unlink("/xmacek27.CUSTOMERNUM4");
    // file
    fclose(file);

    // shared memory
    munmap(NUMBER, sizeof(int));
    munmap(CLOSEDOFFICE, sizeof(bool));
    munmap(CUSTOMER1, sizeof(int));
    munmap(CUSTOMER2, sizeof(int));
    munmap(CUSTOMER3, sizeof(int));
    munmap(CUSTOMERS, sizeof(int)); 
}

void Customer(int id, int TZ) {
    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: Z %d: started\n",*NUMBER, id);
    sem_post(SEMPRINT);

    // Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TZ>
    usleep((rand() % TZ) * 1000);

//--------------------------------------- ZAVRENA ----------------------------------------------------

    // • Pokud je pošta uzavřena
    // ◦ Vypíše: A: Z idZ: going home 
    // ◦ Proces končí
    sem_wait(CLOSEDOFFICECHECK);
        if (*CLOSEDOFFICE == true) {
            sem_wait(SEMPRINT);
                *NUMBER += 1;
                fprintf(file, "%d: Z %d: going home\n",*NUMBER, id);
            sem_post(SEMPRINT);
            sem_post(CLOSEDOFFICECHECK);
            return;
        }
    sem_post(CLOSEDOFFICECHECK);

//---------------------------------------- OTEVRENA -----------------------------------------------------
    // • Pokud je pošta otevřená, náhodně vybere činnost X---číslo z intervalu <1,3>
    int X = rand() % 3 + 1 ;
    // ◦ Vypíše: A: Z idZ: entering office for a service X
    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: Z %d: entering office for a service %d\n",*NUMBER, id, X);
    sem_post(SEMPRINT);

    // ◦ Zařadí se do fronty X a čeká na zavolání úředníkem.
    sem_wait(SEMCUSTOMER);
        (*CUSTOMERS) += 1;
        if (X == 1)
        {
            sem_wait(CUSTOMERNUM1);
                *CUSTOMER1 += 1;
            sem_post(CUSTOMERNUM1);
            sem_post(SEMCUSTOMER);

            sem_wait(SERVICE1);

        }
        else if (X == 2)
        {
            sem_wait(CUSTOMERNUM2);
                *CUSTOMER2 += 1;
            sem_post(CUSTOMERNUM2);
            sem_post(SEMCUSTOMER);

            sem_wait(SERVICE2);

        }
        else if (X == 3)
        {
            sem_wait(CUSTOMERNUM3);
                *CUSTOMER3 += 1;
            sem_post(CUSTOMERNUM3);
            sem_post(SEMCUSTOMER);

            sem_wait(SERVICE3);
        }else{
            sem_post(SEMCUSTOMER); // todo this is bad 
        }

    // ◦ Vypíše: Z idZ: called by office worker
    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: Z %d: called by office worker\n",*NUMBER, id);
    sem_post(SEMPRINT);

    // ◦ Následně čeká pomocí volání usleep náhodný čas v intervalu <0,10> (synchronizace s 
    // úředníkem na dokončení žádosti není vyžadována).
    usleep((rand() % 10) * 1000);
    // ◦ Vypíše: A: Z idZ: going home 
    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: Z %d: going home\n",*NUMBER, id);
    sem_post(SEMPRINT);
    // ◦ Proces končí
    return;
    
}

void Postman(int id, int TU) {
    //  Po spuštění vypíše: A: U idU: started
    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: U %d: started\n",*NUMBER, id);
    sem_post(SEMPRINT);

    // [začátek cyklu]
    while (true)
    {
//--------------------------------------------------------------------------------------------------------
        int X;
        // todo
        // • Pokud v žádné frontě nečeká zákazník a pošta je otevřená vypíše
        sem_wait(SEMCUSTOMER);
            sem_wait(CLOSEDOFFICECHECK);
                if((*CUSTOMERS) == 0 && (*CLOSEDOFFICE) == false)
                {
                    sem_post(SEMCUSTOMER);
                    sem_post(CLOSEDOFFICECHECK);
                    // ◦ Vypíše: A: U idU: taking break
                    sem_wait(SEMPRINT);
                        *NUMBER += 1;
                        fprintf(file, "%d: U %d: taking break\n",*NUMBER, id);
                    sem_post(SEMPRINT);

                    // ◦ Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TU>
                    usleep((rand() % TU) * 1000);

                    // ◦ Vypíše: A: U idU: break finished
                    sem_wait(SEMPRINT);
                        *NUMBER += 1;
                        fprintf(file, "%d: U %d: break finished\n",*NUMBER, id);
                    sem_post(SEMPRINT);

                    // ◦ Pokračuje na [začátek cyklu]
                    // sem_post(SEMCUSTOMER);
                    // sem_post(CLOSEDOFFICECHECK);
                    continue;
                }
                else if (*CUSTOMER1 > 0 || *CUSTOMER2 > 0 || *CUSTOMER3 > 0)
                {
                        *CUSTOMERS -= 1;
                        sem_wait(CUSTOMERNUM1);
                            if (*CUSTOMER1 > 0) {
                                *CUSTOMER1 -= 1;
                                X = 1;
                            }
                        sem_post(CUSTOMERNUM1);

                        // X == 2
                        sem_wait(CUSTOMERNUM2);
                            if (*CUSTOMER2 > 0) {
                                *CUSTOMER2 -= 1;
                                X = 2;
                            }
                        sem_post(CUSTOMERNUM2);

                        // X == 3
                        sem_wait(CUSTOMERNUM3);
                            if (*CUSTOMER3 > 0) {
                                *CUSTOMER3 -= 1;
                                X = 3;
                            }
                        sem_post(CUSTOMERNUM3);

                        // ◦ Vypíše: A: U idU: serving a service of type X
                        sem_wait(SEMPRINT);
                            *NUMBER += 1;
                            fprintf(file, "%d: U %d: serving a service of type %d\n",*NUMBER, id, X);
                        sem_post(SEMPRINT);

                        // ◦ Následně čeká pomocí volání usleep náhodný čas v intervalu <0,10>
                        sem_post(SEMCUSTOMER);
                        sem_post(CLOSEDOFFICECHECK);

                        usleep((rand() % 10) * 1000);

                        if (X == 1)
                        {
                            sem_post(SERVICE1);
                        }
                        else if (X == 2)
                        {
                            sem_post(SERVICE2);
                        }
                        else if (X == 3)
                        {
                            sem_post(SERVICE3);
                        }

                        // ◦ Vypíše: A: U idU: service finished
                        sem_wait(SEMPRINT);
                            *NUMBER += 1;
                            fprintf(file, "%d: U %d: service finished\n",*NUMBER, id);
                        sem_post(SEMPRINT);

                        // ◦ Pokračuje na [začátek cyklu]

                        continue;
                }else
                {
                    // • Pokud v žádné frontě nečeká zákazník a pošta je zavřená
                    // ◦ Vypíše: A: U idU: going home
                    // ◦ Proces končí
                    sem_post(SEMCUSTOMER);
                    sem_post(CLOSEDOFFICECHECK);

                    sem_wait(SEMPRINT);
                        *NUMBER += 1;
                        fprintf(file, "%d: U %d: going home\n",*NUMBER, id);
                    sem_post(SEMPRINT);
                    return;
                }
        // sem_post(CLOSEDOFFICECHECK);
        // sem_post(SEMCUSTOMER);
    }
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
    (*CLOSEDOFFICE) = false;
    *CUSTOMER1 = 0;
    *CUSTOMER2 = 0;
    *CUSTOMER3 = 0;
    *CUSTOMERS = 0;
    
//------------------------------ Main Functionality ----------------------------------------------
// • NZ: počet zákazníků
// • NU: počet úředníků
// • TZ: Maximální čas v milisekundách, po který zákazník po svém vytvoření čeká, než vejde na poštu (eventuálně odchází s nepořízenou).
// • TU: Maximální délka přestávky úředníka v milisekundách.
// • F: Maximální čas v milisekundách, po kterém je uzavřena pošta pro nově příchozí. 

    // creating processes for customers
    pid_t pid = getpid();
    srand(time(NULL) + pid);
	for (int i=0; i < NZ; i++)
	{
        pid = fork();
        srand(time(NULL) + i);

        if(pid == 0 ){
            Customer(i, TZ);
            exit(0);
        }
	}

    // creating processes for postmen
	for (int i=0; i < NU; i++)
    {
        pid = fork();
        srand(time(NULL) + i);

        if (pid == 0) {
            Postman(i, TU);
            exit(0);
        }
    }
    // wait for all processes to finish
    usleep(((rand() % (F/2)) + F/2)* 1000);

    // closing office
    sem_wait(CLOSEDOFFICECHECK);
        (*CLOSEDOFFICE) = true;
    sem_post(CLOSEDOFFICECHECK);

    sem_wait(SEMPRINT);
        *NUMBER += 1;
        fprintf(file, "%d: closing\n", (*NUMBER));
    sem_post(SEMPRINT);

    // waiting for all processes to finish
    while (wait(NULL) > 0){;}

    // todo some waiting untill all processes finish

// -----------------------------------------------------------------------------------------------

    // Clean up
	CleanAll();

    return 0;
}
