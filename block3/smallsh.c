/*********************
 * Mario Bocaletti
 * cs344
 * smallsh: basic shell program
 * supporting cd, exit, and status
 *********************/
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>


/*********************
 * getInput(): gets user selection 
 *********************/
char* getInput() {
  const int MAX_CHARS = 2048;
  int valid = 0;
  int charsEntered = -5;
  int idx = 0;
  size_t bufferSize = 2050;
  char* line = NULL;
  char nextChar = 0;
  while(valid < 1) {
    printf(": ");
    fflush(stdout);
    charsEntered = getline(&line, &bufferSize, stdin);
    /*loop back if length is invalid*/
    if (charsEntered > MAX_CHARS + 1) {
      memset(line,'\0',MAX_CHARS + 2);
      fflush(stdout);
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
      valid = 1;
    }
  }
  return line;
}

/*********************
 * main()
 * *******************/
int main() {
  char* command = 0;
  command = getInput(command);
  printf("\t%s\n",command);
  free(command);
}
