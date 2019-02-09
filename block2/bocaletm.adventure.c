/*********************
 * Mario Bocaletti
 * cs344
 * adventure: reads room files
 * and simulates game
 *********************/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

const char* const ROOMS[] = {"lobby", "cafe", "bank", "restroom", "office", "garage", "lounge", "pool", "elevator", "helipad"};

struct Room {
  char name[10];
  char type[10];
  int** connections;
};

struct Game {
  int steps;
  char** path;
}

/*********************
 * findStart(): finds the room
 * to start the game. Edits path to file
 *********************/
void findStart(char* filepath) {

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





}

