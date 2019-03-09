/*********************
 * Mario Bocaletti
 * cs344
 * keygen: generates a key for
 * encoding/decoding text
 *********************/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

const int ASCII_A = 65;
const int ASCII_Z = 90;
const int ASCII_SPC = 32;
const int maxChars = 27;

/**************************
 * shuffle()
 * Fisher-Yates algorithm to
 * create random permutations of
 * letters represented by ascii integers
 * ************************/
void shuffle(int* numsArray, int n) {
    int i;
    for (i = n - 1; i > 0; i--) {
        int randIdx = rand() % (i+1);
        int temp = numsArray[i];
        numsArray[i] = numsArray[randIdx];
        numsArray[randIdx] = temp;
    } 
}

/*****************************
 * main()
 * ***************************/
int main(int argc, char* argv[]) {
    //seed rand
    srand(time(NULL));

    int keyLength = atoi(argv[1]);

    //populate an array with all the possible ascii codes
    int* numsArray = 0;
    numsArray = malloc(maxChars * sizeof(int));
    int c;
    int idx = 0;
    for (c  = ASCII_A - 1; c <= ASCII_Z; c++) {
        if (idx == 0) {
            numsArray[idx] = ASCII_SPC;
        } else {
            numsArray[idx] = c;
        }
        if (numsArray[idx] == 0) {
            fprintf(stderr,"malloc error in main()\n");
            exit(1);
        }
        idx++;
    }

    //randomize alphabet
    shuffle(numsArray,maxChars);

    //print characters from the random alphabet
    for (idx = 0; idx < keyLength; idx++) {
        printf("%c",numsArray[idx % maxChars]);
        shuffle(numsArray,maxChars);
    }
    printf("\n");

    //free memory
    free(numsArray);
    return 0;
}
