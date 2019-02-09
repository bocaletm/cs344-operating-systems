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


typedef struct  {
  char name[10];
  char type[10];
  int** connections;
} Room;

typedef struct {
  int steps;
  int idx;
  char** path;
} Game; 

/********************
 * checkMem(): checks if malloc
 * succeeded
 * ******************/
checkMem(char* ptr) {
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
  printf("new dir name: %s", newestDirName);
  memcpy(dirName,newestDirName,256 * sizeof(char));
}
/*********************
 * findStart(): finds the room
 * to start the game. Edits path to file
 *********************/
void findStart(char* filepath) {
  /*find newest directory*/
  char* dirName = 0;
  dirName = malloc(256 * sizeof(char));
  checkMem(dirName);
  getDir(dirName);
  printf("dirname:%s\n");

  /*set filepath of start room*/

  /*get first room name*/

  /*append room name to directory*/

  /*open file*/

  /*check file type*/

  /*set filepath*/
}

/*********************
 * readRoom(): reads the room
 *********************/
void readRoom(char* filepath, struct Room* room) {

}

/*********************
 * printRoom(): prints the room
 *********************/
void printRoom(struct Room* room) {

}

/*********************
 * getInput(): gets user selection 
 *********************/
void getInput(char* input) {

}

/*********************
 * endGame(): displays end of game
 * *******************/
void endGame(struct Game* game) {

}

/*********************
 * main()
 * *******************/
int main() {

  Game newGame;
  newGame.steps = 0;
  newGame.idx = 0;
  newGame.path = malloc(CREATED_ROOMS * sizeof(char*));
  checkMem(newGame.path);

  char* filepath = 0;
  filepath = malloc(50 * sizeof(char));
  checkMem(filepath);

  findStart(filepath);
}

