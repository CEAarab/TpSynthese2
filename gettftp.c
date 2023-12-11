#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        // NUmber of the arguments used ( Verification )
        fprintf(stderr, "Usage: %s host file\n", argv[0]);
        return 1;
    }

    char *host = argv[1]; // Getting the first argument (Hostname)
    char *file = argv[2]; // Getting the seconde argument (Name of the file)


    return 0;
}

