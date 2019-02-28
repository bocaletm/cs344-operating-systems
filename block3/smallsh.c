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
#include <unistd.h>
#include <signal.h>

int toggleBackgroundProc = 1;
const int MAX_CHARS = 2048;

/*********************
 * sigstpHandler(): handles ctrl-z 
 *********************/
void sigstpHandler(int signo) {
  char* message = "\nctrl-z switching states...";
  write(STDOUT_FILENO, message, 28);
  toggleBackgroundProc = 1 - toggleBackgroundProc;
  sleep(1);
}

/*********************
 * execute(): performs I/O redirection and exec 
 *********************/
void execute(command) {
    exec(command);
}
/*********************
 * exitGracefully(): kills any processes and jobs
 * and exits zero 
 *********************/
void exitGracefully() {
  exit(0);
}

/*********************
 * changeDir(): cd command 
 *********************/
void changeDir(char* dir) {
  
}

/*********************
 * processCmd(): processes string into commands 
 *********************/
void processCmd(char* rawCmd) {
  char* cmd;
  int lastChar = -1;
  //get first string
  for (i = 0; i < MAX_CHARS; i++) {
    if (command[i] == ' ') {
        break;
    }
    lastChar++;
  } 
  cmd = malloc((lastChar + 1) * sizeof(char));
  memset(cmd,'\0',sizeof(cmd));
  for (i = 0; i <= lastChar; i++) {
      cmd[i] = rawCmd[i];
  }

  //check first string to see if it's native cmd
  if (strcmp(cmd,"exit") == 0) {
    exitGracefully();
  } else if (strcmp(cmd,"cd") == 0) {
    changeDir(rawCmd);
  } else if (strcmp(cmd,"status") == 0) {
    getStatus();
  }
  //run anything non-native
  execute(rawCmd);
} 
/*********************
 * getInput(): gets user selection 
 *********************/
char* getInput() {
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
  //ignore ctrl+c
  signal(SIGINT,SIG_IGN);
  //handle ctrl+z
  struct sigaction sa_sigtstp = {0};
  sa_sigtstp.sa_handler = sigstpHandler; 
  sigfillset(&sa_sigtstp.sa_mask);
  sa_sigtstp.sa_flags = 0;
  //endless loop; exit will terminate shell from
  //processCmd function
  while (1 == 1) {
    char* command = 0;
    command = getInput(command);
    processCmd(command); 
    if (command != 0) {
      free(command);
    }
  }
}
