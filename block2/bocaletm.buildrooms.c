/**************************
 * Mario Bocaletti
 * buldrooms
 * This program generates text files
 * representing rooms
 * ************************/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define NUM_ROOMS 10
#define ROOMS_TO_CREATE 7
const char* const ROOMS[] = {"lobby", "cafe", "bank", "restroom", "office", "garage", "lounge", "pool", "elevator", "helipad"};

/**************************
 * shuffle()
 * Fisher-Yates algorithm to
 * create random permutations of
 * indices to the rooms array
 * ************************/
void shuffle(int* numsArray) {
  srand(time(NULL));
  int i;
  for (i = NUM_ROOMS - 1; i > 0; i--) {
    int randIdx = rand() % (i+1);
    int temp = numsArray[i];
    numsArray[i] = numsArray[randIdx];
    numsArray[randIdx] = temp;
  } 
}
/**************************
 * createRoomFiles()
 * This function generates text files
 * representing rooms
 * ************************/

int createRoomFiles(char* dirName,char* tempStrPtr,char** filepaths){
  /*get random room indexes from global array*/
  int* numsArray = 0;
  numsArray = malloc(NUM_ROOMS * sizeof(int)); 
  int i;
  for (i = 0; i < NUM_ROOMS; i++) {
    numsArray[i] = i;
  }
  shuffle(numsArray);

  /*create the files*/
  int file_descriptor;
  for (i = 0; i < ROOMS_TO_CREATE; i++) {
    memset(tempStrPtr,'\0',strlen(tempStrPtr));
    sprintf(tempStrPtr,"%s/%s",dirName,ROOMS[numsArray[i]]);
    file_descriptor = open(tempStrPtr, O_WRONLY | O_CREAT, 0600);
    
    /*save the path to the files*/
    filepaths[i] = malloc(strlen(tempStrPtr) * sizeof(char));
    memset(filepaths[i], '\0', strlen(tempStrPtr));
    strcpy(filepaths[i],tempStrPtr);
  }

  /*free memory*/
  free(numsArray);

  return 0;
}

/**************************
 * addConnections()
 * This function prints connections 
 * to other rooms in the files
 * ************************/

int addConnections() {
    return 0;
}

/**************************
 * addConnections()
 * This function prints the room types 
 * to the file rooms 
 * ************************/

int addTypes() {
    return 0;
}

int createDirectory(char* dirName){
    /*add the process ID to the name */
    sprintf(dirName,"bocaletm.rooms.%d",getpid());
    int result = mkdir(dirName,0755);
    if (result != 0){
        printf("Error creating directory...\n");
        return 1;
    }
    return 0;
}

int main()
{
     /*set the dir name*/
    char* dirName = 0;
    const int dirNameLength = 50;
    dirName = malloc(dirNameLength * sizeof(char));
    if (dirName == 0) {
        printf("Error. Could not create directory name.\n");
        return 1;
    }
    memset(dirName,'\0',dirNameLength);
    
    /*temp string to hold file names*/
    char* tempStrPtr = 0;
    const int tempStrLength = 60;
    tempStrPtr = malloc(tempStrLength * sizeof(char));
    if (tempStrPtr == 0) {
        printf("Malloc error in temp directory string");
        return 1;
    }
    memset(tempStrPtr,'\0',tempStrLength);

    /*create dir*/
    int directory = createDirectory(dirName);

    /*create files*/
    char** filepaths = 0;
    filepaths = malloc(ROOMS_TO_CREATE * sizeof(char*));
    createRoomFiles(dirName,tempStrPtr,filepaths);

    /*clean up pointers and memory*/
    int i;
    for (i = 0; i < ROOMS_TO_CREATE; i++) {
        printf("%s\n",filepaths[i]);
        free(filepaths[i]); 
    }
 
    free(filepaths);
    free(dirName);
    free(tempStrPtr);
    tempStrPtr = 0;
    filepaths = 0;
    dirName = 0;

    return 0;
}
