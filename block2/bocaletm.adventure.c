/*********************
 * Mario Bocaletti
 * cs344
 * adventure: reads room files
 * and simulates game
 *********************/
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define CREATED_ROOMS 7
#define NUM_ROOMS 10
#define MAX_RM_CHARS 5

const char* const ROOMS[] = {"lobby", "caffe", "bank_", "restr", "offic", "garag", "loung", "pool_", "eleva", "helip"};

typedef struct {
  int steps;
  char** path;
  char** currConnections;
  int currConnectionsCount;
} Game; 

/********************
 * checkMem(): checks if malloc
 * succeeded
 * ******************/
void checkMem(char* ptr) {
  if (ptr == 0) {
      printf("Malloc error.");
      exit(1);
  }
}

/*********************
 * getDir(): finds newest directory
 * *******************/
void getDir(char* dirName) {
  /*set vars*/
  int newestDirTime = -1;
  char dirPrefix[32] = "bocaletm.rooms.";
  char newestDirName[256];
  memset(newestDirName,'\0',sizeof(newestDirName));
  DIR* dirToCheck;
  struct dirent *fileInDir;
  struct stat dirAttributes;
  /*check present directory*/
  dirToCheck = opendir(".");
  if (dirToCheck > 0) {
    while ((fileInDir = readdir(dirToCheck)) != NULL) {
      if (strstr(fileInDir->d_name,dirPrefix)) {
        stat(fileInDir->d_name,&dirAttributes);
        if ((int)dirAttributes.st_mtime > newestDirTime) {
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName,'\0',sizeof(newestDirName));
          strcpy(newestDirName,fileInDir->d_name);
        }
      }
    }
  } else {
    printf("Error. Could not open present directory.\n");
    exit(1);
  }
  closedir(dirToCheck);
  /*copy newestDirName to parameter*/
  memset(dirName,'\0',256 * sizeof(char));
  sprintf(dirName,"./%s",newestDirName);
}
/*********************
 * findStart(): finds the room
 * to start the game. Edits path to file
 *********************/
void findStartEnd(char* start_filepath, char* end_filepath) {
  /*find newest directory*/
  char* dirName = 0;
  dirName = malloc(256 * sizeof(char));
  checkMem(dirName);
  getDir(dirName);

  /*set vars*/
  char tempFilepath[256];
  char readBuffer[256];
  DIR* dirToCheck;
  struct dirent *fileInDir;
  int file_descriptor = 0;
  int nread = 0;
  /*check present directory*/
  dirToCheck = opendir(dirName);
  if (dirToCheck > 0) {
    while ((fileInDir = readdir(dirToCheck)) != NULL) {
      memset(readBuffer,'\0',sizeof(readBuffer));
      memset(tempFilepath,'\0',sizeof(tempFilepath));
      /*get the file path*/
      sprintf(tempFilepath,"%s/%s",dirName,fileInDir->d_name);
      /*open the file*/
      file_descriptor = open(tempFilepath, O_RDONLY, 0600);
      if (file_descriptor < 0) {
        printf("Could not read %s\n",tempFilepath);
        exit(1);
      }
      /*go to end of file*/
      lseek(file_descriptor,-11,SEEK_END);
      /*read the last chars in last line*/
      nread = read(file_descriptor,readBuffer,sizeof(readBuffer));
      if (strstr(readBuffer,"START")) {
        strcpy(start_filepath,tempFilepath);
      } else if (strstr(readBuffer,"END")) {
        strcpy(end_filepath,tempFilepath);
      }
    }
  } else {
    printf("Error. Could not open directory %s.\n",dirName);
    exit(1);
  }
  closedir(dirToCheck);
}
/*********************
 * getInput(): gets user selection 
 *********************/
void getInput(Game* newGame,char* roomInfo) {
  int valid = 1;
  int charsEntered = -5;
  int currChar = -5;
  size_t bufferSize = 0;
  char* line = NULL;
  while(valid > 0) {
    valid = 0;
    printf("WHERE TO? >");
    charsEntered = getline(&line, &bufferSize, stdin);
    if (charsEntered != MAX_RM_CHARS + 1) {
      valid = 1;
      printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
      printf("%s",roomInfo);
      continue;
    }
  }
  printf("%s",line);
  free(line); 
  line = NULL; 
}

/*********************
 * printRoom(): prints the room
 *********************/
void printRoom(char* selection_filepath, Game* newGame) {
  /*store the output to display again in case of
   * bad user input to avoid processing file again*/
  char* output;
  int outputSize = 256;
  output = malloc(outputSize * sizeof(char));
  checkMem(output);
  memset(output,'\0',outputSize);

  FILE* file_ptr = 0;
  /*open the file*/
  file_ptr = fopen(selection_filepath,"r");
  if (file_ptr == 0) {
    printf("Could not open file %s\n",selection_filepath);
    exit(1);
  }
  int nameLength = 6;
  /*read the name of the room*/
  char name[nameLength];
  memset(name,'\0',sizeof(name));
  
  /*move to start of name*/
  fseek(file_ptr,11,SEEK_SET);
  /*read rest of line*/
  fgets(name,nameLength,file_ptr);
  sprintf(output,"CURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ",name);
  
  char** connections = 0;
  connections = malloc(CREATED_ROOMS * sizeof(char));
  char* tempStr = 0;
  int numConnections = 0;
  int tempStrLength = 10;
  /*read connections*/
  int buffSize = 100;
  int i;
  char data[buffSize];
  
  memset(data,'\0',sizeof(data));
  while (fgets(data,buffSize,file_ptr)) {
    char* token = strtok(data," ");
    if (strcmp(token,"CONNECTION") == 0) {
      token = strtok(NULL," ");
      token = strtok(NULL,"\n");
    }
    for (i=0; i < NUM_ROOMS; i++) {
      if (strcmp(token,ROOMS[i]) == 0) {
        tempStr = malloc(tempStrLength * sizeof(char));
        checkMem(tempStr);
        memset(tempStr,'\0',tempStrLength);
        strcpy(tempStr,token);
        connections[numConnections] = tempStr;
        numConnections++;
      }
    }
  }
  /*start at one to avoid empty string from tokenizer*/
  for (i = 0; i < numConnections; i++) {
    sprintf(output,"%s%s",output,connections[i]);
    if (i != numConnections - 1) {
      sprintf(output,"%s, ",output);
    } else {
      sprintf(output,"%s\n",output);
    }
  }
  printf("%s",output);
  /*add connections and count to game struct*/
  newGame->currConnections = connections;
  newGame->currConnectionsCount = numConnections;
  getInput(newGame,output);
}


/*********************
 * endGame(): displays end of game
 * *******************/
void endGame(Game* game) {

}

/*********************
 * main()
 * *******************/
int main() {

  Game* newGame = 0;
  newGame = malloc(sizeof(Game));
  if (newGame == 0) {
    printf("Malloc error creating new Game\n");
    exit(1);
  }
  newGame->steps = 0;
  newGame->path = malloc(CREATED_ROOMS * sizeof(char*));
  checkMem(*newGame->path);
  newGame->currConnections = malloc(CREATED_ROOMS * sizeof(char*));
  newGame->currConnectionsCount = 0;

  char* start_filepath = 0;
  start_filepath = malloc(50 * sizeof(char));
  checkMem(start_filepath);
  
  char* end_filepath = 0;
  end_filepath = malloc(50 * sizeof(char));
  checkMem(end_filepath);
  
  char* selection_filepath = 0;
  selection_filepath = malloc(50 * sizeof(char));
  checkMem(selection_filepath);

  findStartEnd(start_filepath,end_filepath);

  memset(selection_filepath,'\0',50 * sizeof(char));
  strcpy(selection_filepath,start_filepath);
  printf("start: %s select: %s\n",start_filepath,selection_filepath);
  printRoom(selection_filepath, newGame);
  int end = 0;
 /* while (end == 0) {
    printRoom(selection_filepath);
    memset(selection_filepath,'\0',50 * sizeof(char));
    getNextRoom(selection_filepath,newGame);
    if (strstr(selection_filepath,end_filepath)) {
      end=1;
      break;
    }
  }
  endGame(newGame);*/
}

