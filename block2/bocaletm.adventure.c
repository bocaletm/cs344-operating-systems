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
const char* const ROOMS[] = {"lobby", "cafe", "bank", "restroom", "office", "garage", "lounge", "pool", "elevator", "helipad"};

typedef struct {
  int steps;
  int room_idx;
  char** rooms;
  char** path;
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
 * getNextRoom(): reads the room
 *********************/
void getNextRoom(char* filepath, Game* newGame) {
  
}

/*********************
 * printRoom(): prints the room
 *********************/
void printRoom(char* selection_filepath) {
  FILE* file_ptr = 0;
  /*open the file*/
  file_ptr = fopen(selection_filepath,"r");
  if (file_ptr == 0) {
    printf("Could not open file %s\n",selection_filepath);
  }
  int nameLength = 6;
  /*read the name of the room*/
  char name[nameLength];
  memset(name,'\0',sizeof(name));
  
  /*move to start of name*/
  fseek(file_ptr,11,SEEK_SET);
  /*read rest of line*/
  fgets(name,nameLength,file_ptr);
  printf("CURRENT LOCATION: %s\n",name);

  /*find out how many connections to read*/
  int connections = 3;
  /*move to next connection*/
  int buffSize = 20;
  int i;
  char data[buffSize];
  char* token = 0;
  char full_token[20];
  printf("POSSIBLE CONNECTIONS: ");
  memset(data,'\0',sizeof(data));
  while (fgets(data,buffSize,file_ptr)) {
    for (i = 1; i <= connections; i++) {
      memset(full_token,'\0',sizeof(full_token));
      sprintf(full_token,"CONNECTION %d",i);
      token = strtok(data,full_token);
      printf("%s",token);
    }
  }

  /*read the connections*/
}

/*********************
 * getInput(): gets user selection 
 *********************/
void getInput(char* input) {

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
  newGame->room_idx = 0;
  newGame->path = malloc(CREATED_ROOMS * sizeof(char*));
  checkMem(*newGame->path);

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
  printRoom(selection_filepath);
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

