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

/*list of possible rooms used to validate strtok output*/
const char* const ROOMS[] = {"lobby", "caffe", "bank_", "restr", "offic", "garag", "loung", "pool_", "eleva", "helip"};

typedef struct {
  int steps;
  char** path;
  int path_idx;
  char** currConnections;
  int currConnectionsCount;
  char* dirName;
  char* nextRoom;
} Game; 

/********************
 * checkMem(): checks if malloc
 * succeeded
 * ******************/
void checkMem(char* ptr,int line) {
  if (ptr == 0) {
      printf("Malloc error in line %dish.",line);
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
void findStartEnd(char* start_filepath, char* end_filepath, Game* newGame) {
  /*find newest directory*/
  char* dirName = 0;
  dirName = malloc(256 * sizeof(char));
  checkMem(dirName,87);
  getDir(dirName);

  /*save directory in game struct*/
  newGame->dirName = dirName;

  /*set vars*/
  char tempFilepath[50];
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
 * addToPath(): adds a room name to the path
 * in the game struct
 *********************/
void addToPath(Game* game,char* name) {
  game->path[game->path_idx] = malloc(MAX_RM_CHARS * sizeof(char));
  checkMem(game->path[game->path_idx],136);
  strcpy(game->path[game->path_idx],name);
  game->path_idx++;
}

/*********************
 * getInput(): gets user selection 
 *********************/
void getInput(Game* newGame,char* roomInfo) {
  int valid = 0;
  int charsEntered = -5;
  int idx = 0;
  int i = 0;
  size_t bufferSize = 0;
  char* line = NULL;
  char nextChar = 0;
  while(valid < 1) {
    printf("WHERE TO? >");
    charsEntered = getline(&line, &bufferSize, stdin);
    if (charsEntered != MAX_RM_CHARS + 1) {
      printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
      printf("%s",roomInfo);
      continue;
    } else {
      
      /*null terminate the input*/
      idx = 0;
      nextChar = line[idx];
      while (nextChar != '\n'){
        idx++;
        nextChar = line[idx];
      }
      line[idx] = '\0';

      /*check if one of the current room's connections was entered*/
      for (i = 0; i < newGame->currConnectionsCount; i++) {
        if (strcmp(newGame->currConnections[i],line) == 0) {
          valid = 1;
        }
      }

      /*print error msg if not*/
      if (valid == 0) {
        printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        printf("%s",roomInfo);
      }
    }
  }
  /*set filepath to selection*/
  char name[MAX_RM_CHARS + 1];
  memset(name,'\0',sizeof(name));
  /*clean out the rest of the buffer from the string*/
  for (i = 0; i < MAX_RM_CHARS; i++) {
    name[i] = line[i];
  }
  memset(newGame->nextRoom,'\0',50);
  sprintf(newGame->nextRoom,"%s/%s",newGame->dirName,name);
  
  /*add the name to the path*/
  addToPath(newGame,name);
  free(line); 
  line = NULL; 
}

/*********************
 * getRoom(): prints the room
 * and calls user input function
 *********************/
void getRoom(Game* newGame) {
  /*store the output to display again in case of
   * bad user input to avoid processing file again*/
  char* output;
  int outputSize = 256;
  output = malloc(outputSize * sizeof(char));
  checkMem(output,213);
  memset(output,'\0',outputSize);

  FILE* file_ptr = 0;
  /*open the file*/
  file_ptr = fopen(newGame->nextRoom,"r");
  if (file_ptr == 0) {
    printf("Could not open file %s\n",newGame->nextRoom);
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
 
  /*read file line by line*/
  memset(data,'\0',sizeof(data));
  while (fgets(data,buffSize,file_ptr)) {
    /*use strtok to clean up line*/
    char* token = strtok(data," ");
    if (strcmp(token,"CONNECTION") == 0) {
      token = strtok(NULL," ");
      token = strtok(NULL,"\n");
    }

    /*add each room name to the array if it's in the global names array*/
    for (i=0; i < NUM_ROOMS; i++) {
      if (strcmp(token,ROOMS[i]) == 0) {
        tempStr = malloc(tempStrLength * sizeof(char));
        checkMem(tempStr,258);
        memset(tempStr,'\0',tempStrLength);
        strcpy(tempStr,token);
        connections[numConnections] = tempStr;
        numConnections++;
      }
    }
  }

  /*create an output string, so we don't have to re-read file data*/
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
  printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
  printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",game->steps);
  int i;
  for (i = 0; i < game->path_idx; i++) {
    printf("%s\n",game->path[i]);
  }
}

/*********************
 * main()
 * *******************/
int main() {
  /*initialize all game struct vars*/
  Game* newGame = 0;
  newGame = malloc(sizeof(Game));
  if (newGame == 0) {
    printf("Malloc error creating new Game\n");
    exit(1);
  }
  newGame->steps = 0;
  newGame->path = 0;
  newGame->path = malloc(CREATED_ROOMS * sizeof(char*));
  if (newGame->path == 0) {
    printf("Malloc error creating path\n");
    exit(1);
  }
  newGame->currConnections = malloc(CREATED_ROOMS * sizeof(char*));
  newGame->currConnectionsCount = 0;
  newGame->dirName = 0;
  newGame->nextRoom = 0;
  newGame->path_idx = 0;


  /*strings to hold path to files*/
  char* start_filepath = 0;
  start_filepath = malloc(50 * sizeof(char));
  checkMem(start_filepath,322);
  memset(start_filepath,'\n',50);
  
  char* end_filepath = 0;
  end_filepath = malloc(50 * sizeof(char));
  checkMem(end_filepath,327);
  memset(end_filepath,'\n',50);
  
  findStartEnd(start_filepath,end_filepath,newGame);

  newGame->nextRoom = start_filepath;
  
  while (strcmp(newGame->nextRoom,end_filepath) != 0) {
    getRoom(newGame);
    newGame->steps++;
    printf("\n");
  }
  endGame(newGame);
}

