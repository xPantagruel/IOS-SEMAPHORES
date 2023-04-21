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
sem_t *MUTEX = NULL;
sem_t *SEMPRINT = NULL;
sem_t *SEMCLOSEDOFFICE = NULL;
sem_t *SEMCUSTOMER1 = NULL;
sem_t *SEMCUSTOMER2 = NULL;
sem_t *SEMCUSTOMER3 = NULL;
sem_t *SEMPOSTMAN = NULL;
sem_t *SEMCUSTOMERDONE = NULL;
sem_t *SEMPOSTMANDONE = NULL;

// shared memory
int *NUMBER = NULL;
bool *CLOSEDOFFICE = NULL;
int *CUSTOMERS = NULL;
int *CUSTOMER1 = NULL;
int *CUSTOMER2 = NULL;
int *CUSTOMER3 = NULL;


/**
 * @brief Initialize semaphores and shared memory
 * 
 * @return true 
 * @return false 
 */
bool Initialize() {
    // semaphores
    MUTEX = sem_open("/xmacek27.MUTEX",  O_CREAT | O_EXCL, 0666, 1);
    if (MUTEX == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() MUTEX failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMPRINT = sem_open("/xmacek27.SEMPRINT",  O_CREAT | O_EXCL, 0666, 1);
    if (SEMPRINT == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMPRINT failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMCLOSEDOFFICE = sem_open("/xmacek27.SEMCLOSEDOFFICE",  O_CREAT | O_EXCL, 0666, 1);
    if (SEMCLOSEDOFFICE == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMCLOSEDOFFICE failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMCUSTOMER1 = sem_open("/xmacek27.SEMCUSTOMER1",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMCUSTOMER1 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMCUSTOMER1 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMCUSTOMER2 = sem_open("/xmacek27.SEMCUSTOMER2",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMCUSTOMER2 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMCUSTOMER2 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMCUSTOMER3 = sem_open("/xmacek27.SEMCUSTOMER3",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMCUSTOMER3 == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMCUSTOMER3 failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMPOSTMAN = sem_open("/xmacek27.SEMPOSTMAN",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMPOSTMAN == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMPOSTMAN failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMCUSTOMERDONE = sem_open("/xmacek27.SEMCUSTOMERDONE",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMCUSTOMERDONE == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMCUSTOMERDONE failed with error code %d: %s\n", errno, strerror(errno));
        return false;
    }

    SEMPOSTMANDONE = sem_open("/xmacek27.SEMPOSTMANDONE",  O_CREAT | O_EXCL, 0666, 0);
    if (SEMPOSTMANDONE == SEM_FAILED) {
        fprintf(stderr, "Error: sem_open() SEMPOSTMANDONE failed with error code %d: %s\n", errno, strerror(errno));
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

    if ((CUSTOMERS = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed NUMBER\n");
        return false;
    }
    
    if ((CUSTOMER1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed CUSTOMER1\n");
        return false;
    }

    if ((CUSTOMER2 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed CUSTOMER2\n");
        return false;
    }

    if ((CUSTOMER3 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "Error: mmap() failed CUSTOMER3\n");
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
    sem_close(MUTEX);
    sem_unlink("/xmacek27.MUTEX");

    sem_close(SEMPRINT);
    sem_unlink("/xmacek27.SEMPRINT");

    sem_close(SEMCLOSEDOFFICE);
    sem_unlink("/xmacek27.SEMCLOSEDOFFICE");

    sem_close(SEMCUSTOMER1);
    sem_unlink("/xmacek27.SEMCUSTOMER1");

    sem_close(SEMCUSTOMER2);
    sem_unlink("/xmacek27.SEMCUSTOMER2");

    sem_close(SEMCUSTOMER3);
    sem_unlink("/xmacek27.SEMCUSTOMER3");

    sem_close(SEMPOSTMAN);
    sem_unlink("/xmacek27.SEMPOSTMAN");

    sem_close(SEMCUSTOMERDONE);
    sem_unlink("/xmacek27.SEMCUSTOMERDONE");

    sem_close(SEMPOSTMANDONE);
    sem_unlink("/xmacek27.SEMPOSTMANDONE");
    // file
    fclose(file);

    // shared memory
    munmap(NUMBER, sizeof(int));
    munmap(CLOSEDOFFICE, sizeof(bool));
    munmap(CUSTOMERS, sizeof(int)); 
    munmap(CUSTOMER1, sizeof(int));
    munmap(CUSTOMER2, sizeof(int));
    munmap(CUSTOMER3, sizeof(int));
}

void Customer(int id, int TZ) {
    sem_wait(SEMPRINT);
        (*NUMBER) += 1;
        fprintf(file, "%d: Z %d: started\n", (*NUMBER), id);
    sem_post(SEMPRINT);

    //Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TZ>
    if(TZ != 0)
    {
        usleep((rand() % TZ) * 1000);
    }

// ------------------------POSTA UZAVRENA--------------------------------------------------
    bool closed = false;
    sem_wait(SEMCLOSEDOFFICE);
        closed = (*CLOSEDOFFICE);
    sem_post(SEMCLOSEDOFFICE);

    if(closed){
        sem_wait(SEMPRINT);
            (*NUMBER) += 1;
            fprintf(file, "%d: Z %d: going home\n", (*NUMBER), id);
        sem_post(SEMPRINT);
        return;
    }
// --------------------------------------------------------------------------------------

// ------------------------POSTA OTEVRENA--------------------------------------------------
//náhodně vybere činnost X---číslo z intervalu <1,3>
    int X = rand() % 3 + 1;

    sem_wait(MUTEX);
        (*CUSTOMERS) += 1;

        if(X == 1) 
        {   
            (*CUSTOMER1) += 1;
        }
        else if(X == 2)
        {
            (*CUSTOMER2) += 1;
        }
        else if(X == 3)
        {
            (*CUSTOMER3) += 1;
        }
    sem_post(MUTEX);

    sem_wait(SEMPRINT);
        (*NUMBER) += 1;
        fprintf(file, "%d: Z %d: entering office for a service %d\n", (*NUMBER), id, X);
    sem_post(SEMPRINT);

    // todo zvednuti hodnoty kolik je zákazníků typu X
    if(X == 1) 
    {   
        sem_post(SEMCUSTOMER1);
    }else if(X == 2) 
    {   
        sem_post(SEMCUSTOMER2);
    }else if(X == 3)
    {
        sem_post(SEMCUSTOMER3);
    }
    sem_wait(SEMPOSTMAN);

    sem_wait(SEMPRINT);
        (*NUMBER) += 1;
        fprintf(file, "%d: Z %d: called by office worker\n", (*NUMBER), id);
    sem_post(SEMPRINT);

    sem_post(SEMCUSTOMERDONE);
    sem_wait(SEMPOSTMANDONE);

// --------------------------------------------------------------------------------------
    //  Následně čeká pomocí volání usleep náhodný čas v intervalu <0,10>
    usleep((rand() % 10) * 1000);

    sem_wait(SEMPRINT);
        (*NUMBER) += 1;
        fprintf(file, "%d: Z %d: going home\n", (*NUMBER), id);
    sem_post(SEMPRINT);
    return;
}

void Postman(int id, int TU) {
    sem_wait(SEMPRINT);
        (*NUMBER) += 1;
        fprintf(file, "%d: U %d: started\n", (*NUMBER), id);
    sem_post(SEMPRINT);

    while (true) {
        int X = rand() % 3 + 1;

//-------- jedna z front neni prazdna
        sem_wait(MUTEX);
            bool IHaveCustomer = false;
            if(*CUSTOMERS > 0){
                // potrebuju snizit pocet customers a ziskat jeste ktereho budu snizovat 
                *CUSTOMERS -= 1;

                while (true)
                {
                    if(X == 1 && (*CUSTOMER1) > 0){
                        (*CUSTOMER1) -= 1;
                        break;
                    }
                    if(X == 2 && (*CUSTOMER2) > 0){
                        (*CUSTOMER2) -= 1;
                        break;
                    }
                    if(X == 3 && (*CUSTOMER3) > 0){
                        (*CUSTOMER3) -= 1;
                        break;
                    }
                    X = rand() % 3 + 1;
                }
                IHaveCustomer = true;
            }
        sem_post(MUTEX);

        if(IHaveCustomer)
        {    
            if(X == 1)
            {
                sem_wait(SEMCUSTOMER1);
            }
            else if (X == 2 )
            {
                sem_wait(SEMCUSTOMER2);
            }
            else if( X == 3)
            {
                sem_wait(SEMCUSTOMER3);
            }
            sem_post(SEMPOSTMAN);

            sem_wait(SEMPRINT);
                (*NUMBER) += 1;
                fprintf(file, "%d: U %d: serving a service of type %d\n", (*NUMBER), id, X);
            sem_post(SEMPRINT);

            //  Následně čeká pomocí volání usleep náhodný čas v intervalu <0,10>
            usleep((rand() % 10) * 1000);

            sem_wait(SEMPRINT);
                (*NUMBER) += 1;
                fprintf(file, "%d: U %d: service finished\n", (*NUMBER), id);
            sem_post(SEMPRINT);

            sem_wait(SEMCUSTOMERDONE);
            sem_post(SEMPOSTMANDONE);
            continue;
        }

//-------- vsechny jsou prazdne a posta je otevrena 
        bool closed = false;
        sem_wait(SEMCLOSEDOFFICE);
            closed = (*CLOSEDOFFICE);
        sem_post(SEMCLOSEDOFFICE);

        if(!closed){
            sem_wait(SEMPRINT);
                (*NUMBER) += 1;
                fprintf(file, "%d: U %d: taking break\n", (*NUMBER), id);
            sem_post(SEMPRINT);

            //  Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TU>
            if (TU != 0)
            {
                usleep((rand() % TU) * 1000);
            }

            sem_wait(SEMPRINT);
                (*NUMBER) += 1;
                fprintf(file, "%d: U %d: break finished\n", (*NUMBER), id);
            sem_post(SEMPRINT);

            continue;
        }

//------ a jinak posta je uzavrena
        sem_wait(SEMPRINT);
            (*NUMBER) += 1;
            fprintf(file, "%d: U %d: going home\n", (*NUMBER), id);
        sem_post(SEMPRINT);
        return;
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
    (*CUSTOMERS) = 0;
    (*CUSTOMER1) = 0;
    (*CUSTOMER2) = 0;
    (*CUSTOMER3) = 0;
    
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
            Customer(i+1, TZ);
            exit(0);
        }
	}

    // creating processes for postmen
	for (int i=0; i < NU; i++)
    {
        pid = fork();
        srand(time(NULL) + i);

        if (pid == 0) {
            Postman(i+1, TU);
            exit(0);
        }
    }
    // wait for all processes to finish
    if (F != 0)
    {
        usleep(((rand() % (F/2)) + F/2)* 1000);
    }
    
    // closing office
    sem_wait(SEMCLOSEDOFFICE);
        (*CLOSEDOFFICE) = true;
    sem_post(SEMCLOSEDOFFICE);

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
