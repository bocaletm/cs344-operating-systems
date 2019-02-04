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
const char* const ROOM_TYPES[] = {"START_ROOM","MID_ROOM","END_ROOM"};

/**************************
 * shuffle()
 * Fisher-Yates algorithm to
 * create random permutations of
 * indices to the rooms array
 * ************************/
void shuffle(int* numsArray, int n) {
  srand(time(NULL));
  int i;
  for (i = n - 1; i > 0; i--) {
    int randIdx = rand() % (i+1);
    int temp = numsArray[i];
    numsArray[i] = numsArray[randIdx];
    numsArray[randIdx] = temp;
  } 
}

/**************************
 * randomRooms()
 * create random permutations of
 * indices to the rooms array
 * ************************/
void randomRooms(int* numsArray, int n) {  
    int i;
    for (i = 0; i < n; i++) {
      numsArray[i] = i;
    }
    shuffle(numsArray,n);
}

/**************************
 * createRoomFiles()
 * This function generates text files
 * representing rooms and writes the room name
 * ************************/

int createRoomFiles(char* dirName,char** filepaths,int* createdRooms){
    
  int* numsArray = 0;
  numsArray = malloc(NUM_ROOMS * sizeof(int)); 
  if (numsArray == 0) {
    printf("Error creating random indexes to randomize rooms.\n");
    exit(1);
  }

  /*filepaths created that need to be freed*/
  int filepathCount = 0;
  
  /*get random room indexes from global array*/
  randomRooms(numsArray,NUM_ROOMS); 
  
  /*temp string to hold file names*/
  char* tempStrPtr = 0;
  const int tempStrLength = 60;
  tempStrPtr = malloc(tempStrLength * sizeof(char));
  if (tempStrPtr == 0) {
      printf("Malloc error in temp filename string");
      free(dirName);
      free(filepaths);
      exit(1);
  }
  memset(tempStrPtr,'\0',tempStrLength);

  /*create the files*/
  int file_descriptor;
  int i;
  for (i = 0; i < ROOMS_TO_CREATE; i++) {
    memset(tempStrPtr,'\0',strlen(tempStrPtr));
    sprintf(tempStrPtr,"%s/%s",dirName,ROOMS[numsArray[i]]);
    file_descriptor = open(tempStrPtr, O_WRONLY | O_CREAT, 0600);
    if (file_descriptor < 0) {
        printf("Could not create %s\n", tempStrPtr);
        return filepathCount;
    }

    /*save the path to the files*/
    filepaths[i] = malloc(strlen(tempStrPtr) * sizeof(char));
    if (filepaths[i] == 0) {
        printf("Malloc error on %s\n", tempStrPtr);
        return filepathCount;
    } else {
        filepathCount++;
    }
    
    memset(filepaths[i],'\0',strlen(tempStrPtr));
    strcpy(filepaths[i],tempStrPtr);

    /*write the name to the files*/
    memset(tempStrPtr,'\0',strlen(tempStrPtr));
    sprintf(tempStrPtr,"ROOM NAME: %s\n",ROOMS[numsArray[i]]);
    write(file_descriptor,tempStrPtr,strlen(tempStrPtr) * sizeof(char));
    close(file_descriptor);

    /*save the index of the created rooms*/
    createdRooms[i] = numsArray[i];
  }

  /*free memory*/
  free(numsArray);
  free(tempStrPtr);
  numsArray = 0;
  tempStrPtr = 0; 
  return filepathCount;
}
/**************************
 * connect()
 * This function connects two rooms 
 * ************************/
void connect() {

}
/**************************
 * addConnections()
 * This function prints connections 
 * to other rooms in the files
 * ************************/

int addConnections(char** filepaths,int* createdRooms) {
    int* randomRoomsPtr = 0;
    randomRoomsPtr = malloc(ROOMS_TO_CREATE * sizeof(int)); 
    if (randomRoomsPtr == 0) {
        printf("Error creating random indexes to randomize rooms.\n");
        exit(1);
    }
    randomRooms(randomRoomsPtr,ROOMS_TO_CREATE);

    /* the number of connections each room
     * in the randomRooms list has */
    int connections[ROOMS_TO_CREATE];
    int i;
    for (i = 0; i < ROOMS_TO_CREATE; i++) {
        connections[i] = 0;
    }
    
    int idx;
    /*iterate through the random rooms and create connections*/
    for (i = 0; i < ROOMS_TO_CREATE; i++) {
        idx = 0;
        while (connections[i] < 3) {
            if (idx == i) {
               idx++;
               continue;
            } else {


        }
    }
    return 0;
}

/**************************
 * addTypes()
 * This function prints the room types 
 * to the file rooms 
 * ************************/

int addTypes(char** filepaths) {
    int file_descriptor = 0;
    /*temp string to hold type string*/
    char* tempStrPtr = 0;
    const int tempStrLength = 60;
    tempStrPtr = malloc(tempStrLength * sizeof(char));
    if (tempStrPtr == 0) {
        printf("Malloc error in temp filename string\n");
        exit(1);
    }
    int i;
    for (i = 0; i < ROOMS_TO_CREATE; i++) {
    /*open file to write*/
        file_descriptor = open(filepaths[i], O_WRONLY | O_APPEND);
        if (file_descriptor < 0) {
            printf("Error. Could not open %s\n",filepaths[i]);
            return 1;
        }
        /*Assign the room type: start, end, mid */
        if (i == 0) {
            memset(tempStrPtr,'\0',tempStrLength);
            sprintf(tempStrPtr,"ROOM TYPE: %s\n",ROOM_TYPES[0]);
            write(file_descriptor,tempStrPtr,strlen(tempStrPtr) * sizeof(char));
        } else if (i == 1) {
            memset(tempStrPtr,'\0',tempStrLength);
            sprintf(tempStrPtr,"ROOM TYPE: %s\n",ROOM_TYPES[2]);
            write(file_descriptor,tempStrPtr,strlen(tempStrPtr) * sizeof(char));
        } else {
            memset(tempStrPtr,'\0',tempStrLength);
            sprintf(tempStrPtr,"ROOM TYPE: %s\n",ROOM_TYPES[1]);
            write(file_descriptor,tempStrPtr,strlen(tempStrPtr) * sizeof(char));
        } 
        close(file_descriptor);
    }
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
        printf("Malloc error creating directory name in main()\n");
        exit(1);
    }
    memset(dirName,'\0',dirNameLength);

    /*create dir*/
    createDirectory(dirName);

    /*create files*/
    char** filepaths = 0;
    filepaths = malloc(ROOMS_TO_CREATE * sizeof(char*));
    if (filepaths == 0) {
        printf("Malloc error creating filepaths in main()\n");
        exit(1);
    }
    memset(filepaths,0,ROOMS_TO_CREATE);

    int* createdRooms = 0;
    createdRooms = malloc(ROOMS_TO_CREATE * sizeof(int));
    if (createdRooms == 0) {
        printf("Malloc error creating createdRooms in main()\n");
        exit(1);
    }
    memset(createdRooms,-1,ROOMS_TO_CREATE);
    
    int filepathCount = createRoomFiles(dirName,filepaths,createdRooms);
   
        /*add connections*/
    if (filepathCount == ROOMS_TO_CREATE) {
        addConnections(filepaths,createdRooms);
    } 

    /*add types*/
    if (filepathCount == ROOMS_TO_CREATE) {
        addTypes(filepaths);
    } 

    /*clean up pointers and memory
    int i;
    for (i = 0; i < filepathCount; i++) {
        free(filepaths[i]); 
    }
    free(dirName);
    dirName = 0;
    free(filepaths);
    filepaths = 0;
    free(createdRooms);
    createdRooms = 0; */
    return 0;
}
