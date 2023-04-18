#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>

// file proj2.out
FILE *file;

// semaphores
sem_t *semaphore;




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


//------------------------------ Main Functionality ----------------------------------------------
// • NZ: počet zákazníků
// • NU: počet úředníků
// • TZ: Maximální čas v milisekundách, po který zákazník po svém vytvoření čeká, než vejde na poštu (eventuálně odchází s nepořízenou).
// • TU: Maximální délka přestávky úředníka v milisekundách.
// • F: Maximální čas v milisekundách, po kterém je uzavřena pošta pro nově příchozí. 

	fclose(file);

    return 0;
}
