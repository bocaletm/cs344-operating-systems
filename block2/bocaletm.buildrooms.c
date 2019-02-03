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

int createRoomFiles(char* dirName,char** filepaths){
    /*filepaths created that need to be freed*/
  int filepathCount = 0;
  /*get random room indexes from global array*/
  int* numsArray = 0;
  numsArray = malloc(NUM_ROOMS * sizeof(int)); 
  if (numsArray == 0) {
    printf("Error creating random indexes to randomize rooms.");
    free(dirName);
    free(filepaths);
    exit(1);
  }

  int i;
  for (i = 0; i < NUM_ROOMS; i++) {
    numsArray[i] = i;
  }
  shuffle(numsArray);

    /*temp string to hold file names*/
  char* tempStrPtr = 0;
  const int tempStrLength = 60;
  tempStrPtr = malloc(tempStrLength * sizeof(char));
  if (tempStrPtr == 0) {
      printf("Malloc error in temp directory string");
      free(dirName);
      free(filepaths);
      exit(1);
  }
  memset(tempStrPtr,'\0',tempStrLength);

  /*create the files*/
  int file_descriptor;
  for (i = 0; i < ROOMS_TO_CREATE; i++) {
    memset(tempStrPtr,'\0',strlen(tempStrPtr));
    sprintf(tempStrPtr,"%s/%s",dirName,ROOMS[numsArray[i]]);
    file_descriptor = open(tempStrPtr, O_WRONLY | O_CREAT, 0600);
    if (file_descriptor < 0) {
        printf("Could not create %s\n", tempStrPtr);
        return filepathCount;
    }
    close(file_descriptor);
    /*save the path to the files*/
    filepaths[i] = malloc(strlen(tempStrPtr) * sizeof(char));
    if (filepaths[i] == 0) {
        printf("Could not create %s\n", tempStrPtr);
        return filepathCount;
    } else {
        filepathCount++;
    }

    memset(filepaths[i], '\0', strlen(tempStrPtr));
    strcpy(filepaths[i],tempStrPtr);
  }

  /*free memory*/
  free(numsArray);
  free(tempStrPtr);
  numsArray = 0;
  tempStrPtr = 0; 
  return filepathCount;
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

void createDirectory(char* dirName){
    /*add the process ID to the name */
    sprintf(dirName,"bocaletm.rooms.%d",getpid());
    int result = mkdir(dirName,0755);
    if (result != 0){
        printf("mkdir error\n");
        perror("In createDirectory()");
        exit(1);
    }
}

int main()
{
     /*set the dir name*/
    char* dirName = 0;
    const int dirNameLength = 50;
    dirName = malloc(dirNameLength * sizeof(char));
    if (dirName == 0) {
        printf("Error. Could not create directory name.\n");
        exit(1);
    }
    memset(dirName,'\0',dirNameLength);

    /*create dir*/
    createDirectory(dirName);

    /*create files*/
    char** filepaths = 0;
    filepaths = malloc(ROOMS_TO_CREATE * sizeof(char*));
    memset(filepaths,0,ROOMS_TO_CREATE);
    int filepathCount = createRoomFiles(dirName,filepaths);

    /*clean up pointers and memory*/
    int i;
    for (i = 0; i < filepathCount; i++) {
        printf("%s\n",filepaths[i]);
        free(filepaths[i]); 
    }
 
    free(filepaths);
    free(dirName);
    filepaths = 0;
    dirName = 0;

    return 0;
}
