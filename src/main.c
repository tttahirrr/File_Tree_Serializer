#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv) {
    int ret;

    if (validargs(argc, argv)) {
        fflush(stdout);
        USAGE(*argv, EXIT_FAILURE);
    }

    if (global_options & 0x1) {
        fflush(stdout);
        USAGE(*argv, EXIT_SUCCESS);
    }

    // check for the serialization flag (-s)
    if (global_options & 0x2) { // assuming 0x2 is the bit for the -s flag
        ret = serialize(); // call serialize function
        if (ret == -1) {
            fflush(stdout);
            return EXIT_FAILURE; // if serialization failed
        }
        fflush(stdout);
        return EXIT_SUCCESS; // if serialization succeeded
    }


    // check for the serialization flag (-d)
    if (global_options & 0x4) { // assuming 0x4 is the bit for the -d flag
        ret = deserialize(); // call deserialize function
        if (ret == -1) {
            fflush(stdout);
            return EXIT_FAILURE; // if deserialization failed
        }
        fflush(stdout);
        return EXIT_SUCCESS; // if deserialization succeeded
    }

    fflush(stdout);
    return EXIT_SUCCESS;
}

/*
* Just a reminder: All non-main functions should
* be in another file not named main.c
*/
