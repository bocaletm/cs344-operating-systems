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

int createRoomFiles(char* dirName){
  const int roomsToCreate = 7;
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
  char* tempStrPtr = 0;
  const int tempStrLength = 60;
  for (i = 0; i < roomsToCreate; i++) {
    tempStrPtr = malloc(tempStrLength * sizeof(char));
    if (tempStrPtr == 0) {
      printf("Malloc error in temp directory string");
      return 1;
    }
    tempStrPtr = strcat(dirName,ROOMS[numsArray[i]]);
    printf("creating %s\n",ROOMS[numsArray[i]]);
    /*file_descriptor = open(tempStrPtr, O_WRONLY | O_CREAT, 0600);*/
    free(tempStrPtr);
  }
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
    int directory = createDirectory(dirName);
    int files = createRoomFiles(dirName);
    free(dirName);
    return 0;
}
