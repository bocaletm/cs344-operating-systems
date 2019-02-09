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
checkMem(char* ptr,char** function) {
  if (ptr == 0) {
      printf("Malloc error in %s\n",function);
      exit(1);
  }
}

/*********************
 * getDir(): finds newest directory
 * *******************/
void getDir(char* dirName) {
  DIR* dirPtr = 0;
  struct dirent* entityPtr = 0;
  dirPtr = opendir("./");
  if (dirPtr != NULL) {
    while(entityPtr == readdir(dirPtr)){
      puts(entityPtr->d_name);
      printf("%s\n",entityPtr->d_name);
    }
    (void) closedir(dirPtr);
  }
}

/*********************
 * findStart(): finds the room
 * to start the game. Edits path to file
 *********************/
void findStart(char* filepath) {
  /*find newest directory*/
  char* dirName = 0;
  dirName = malloc(25 * sizeof(char));
  checkMem(dirName,"findStart()");
  getDir(dirName);

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
  checkMem(newGame.path,"main()");

  char* filepath = 0;
  filepath = malloc(50 * sizeof(char));
  checkMem(filepath,"main()");

  findStart(filepath);
}

