/*********************
 * Mario Bocaletti
 * cs344
 * smallsh: basic shell program
 * supporting cd, exit, and status
 *********************/


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

int toggleBackgroundProc = 1;
const int MAX_CHARS = 2048;
void sigstpHandler(int);
void sigchldHandler(int);

/*********************
 * sigstpHandler(): handles ctrl-z 
 *********************/
void sigstpHandler(int signo) {
  char* message1 = "Exiting foreground-only mode\n";
  char* message2 = "Entering foreground-only mode (& is now ignored)\n";
  switch (toggleBackgroundProc) {
    case 0 :
      write(STDOUT_FILENO,message1,29);
      fflush(stdout);
      break;
    case 1 :
      write(STDOUT_FILENO,message2,49);
      fflush(stdout);
      break;
  }
  toggleBackgroundProc = 1 - toggleBackgroundProc;
}

/*********************
 * sigchldHandler(): handles child process termination 
 *********************/
void sigstpHandler(int signo) {
  pid_t p;
  int status;
  char* message1 = "Child process ";
  char* message2 = "terminated with status ";
  char* enter = "\n";
  /*reap what you sow*/
  while ((p=waitpid(-1,&status,WNOHANG)) != -1) {
    write(STDOUT_FILENO,message1,14);
    write(STDOUT_FILENO,p,sizeof(p));
    write(STDOUT_FILENO,message2,24);
    write(STDOUT_FILENO,status,sizeof(status));
    write(STDOUT_FILENO,enter,1);
    fflush(stdout);
  }
}

/*********************
 * execute(): performs I/O redirection and exec 
 *********************/
void execute(char* command,int* spawnExit) {
  pid_t spawnpid = -5;
  spawnpid = fork();
  switch (spawnpid)
  {
    case -1:
      printf("Fork error. Exiting.\n");
      fflush(stdout);
      exit(1);
      break;
    case 0:
      //reset sigint for the child
      signal(SIGINT,SIG_DFL);
      //execute the command
      




      break;
    default:
      if (toggleBackgroundProc && background) {
        spawnpid = waitpid(spawnpid,spawnExit,WNOHANG);
      } else {
        spawnpid = waitpid(spawnpid,spawnExit,0);
      }
      break;
  }
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
  /*position of cd argument start*/
  int dirStart = 3;
  int err = -5;
  char* token;
  const char space[2] = " "; 
  /*char cwd[1024];
  memset(cwd,'\0',sizeof(cwd));
  getcwd(cwd, sizeof(cwd));
  printf("pwd: %s\n",cwd);
  fflush(stdout);*/
  /*if there's nothing where directory should be
   * cd home */
  if (dir[dirStart] == '\0') {
      err = chdir(getenv("HOME"));
      /*memset(cwd,'\0',sizeof(cwd));
      getcwd(cwd, sizeof(cwd));
      printf("pwd: %s\n",cwd);
      fflush(stdout);*/
  } else {
      /*use second space-delim string; ignore everything else*/
      token = strtok(dir,space);
      token = strtok(NULL,space);
      err = chdir(token);
      /*memset(cwd,'\0',sizeof(cwd));
      getcwd(cwd, sizeof(cwd));
      printf("pwd: %s\n",cwd);
      fflush(stdout);*/
   }
  if (err == -1) {
    printf("Could not find that directory\n");
  }
}

/*********************
 * getStatus(): gets status of last command 
 *********************/
void getStatus() {

}

/*********************
 * processCmd(): processes string into commands 
 *********************/
void processCmd(char* rawCmd) {
  char* cmd;
  int exitStatus = -5;
  int lastChar = -1;
  //get first string
  int i;
  for (i = 0; i < MAX_CHARS; i++) {
    if (rawCmd[i] == ' ' || rawCmd[i] == '\n' || rawCmd == '\0') {
        break;
    }
    lastChar++;
  } 
  cmd = malloc((lastChar + 1) * sizeof(char));
  memset(cmd,'\0',(lastChar+1));
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
  free(cmd);
  //run anything non-native
  execute(rawCmd,&exitStatus);
} 
/*********************
 * getInput(): gets user input and recovers
 * from sigtstp handler during fgets 
 *********************/
char* getInput() {
  int idx = 0;
  char* line = NULL;
  line = malloc((MAX_CHARS + 1) * sizeof(char));
  memset(line,'\0',MAX_CHARS + 2);
  char nextChar = 0;
  /*prompt*/
  printf(": ");
  fflush(stdout);
  /*get input*/
  /*save the toggler to see if fgets was terminated*/
  int toggler = toggleBackgroundProc; 
  fgets(line,(MAX_CHARS + 1),stdin);
  /*mimic behavior of just pressing enter if sigtstp 
   * signal was received during fgets*/
  if (toggler != toggleBackgroundProc) {
    line[0] = '\n';
    line[1] = '\0';
  } else {
    /*null terminate the input*/
    nextChar = line[idx];
    while (nextChar != '\n'){
      if (idx == MAX_CHARS + 1) {
        printf("Command must be <= %d. Characters may be lost.\n",MAX_CHARS);
        fflush(stdout);
        idx--;
        break;
      }
      idx++;
      nextChar = line[idx];
    }
    line[idx] = '\0';
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
  sigaction(SIGTSTP, &sa_sigtstp, NULL);

  //endless loop; exit will terminate shell from
  //processCmd function
  while (1 == 1) {
    char* command = 0;
    command = getInput(command);
    /*skip all processing for comments*/
    if (command[0] != '#') {
      processCmd(command); 
    }
    if (command != 0) {
      free(command);
    }
  }
}
