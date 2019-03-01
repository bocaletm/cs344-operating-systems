/*********************
 * Mario Bocaletti
 * cs344
 * smallsh: basic shell program
 * supporting cd, exit, and status
 *********************/


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

/*globals for handlers*/
int toggleBackgroundProc = 1;
pid_t deadChildPid = NULL;
int deadChildStatus = -5;

/*comand limits*/
const int MAX_CHARS = 2048;
const int MAX_ARGS = 512;

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
void sigchldHandler(int signo) {
  pid_t p;
  int status = -6;
  /*reap what you sow*/
  while ((p=waitpid((pid_t)(-1),&status,WNOHANG)) > 0) {
    deadChildPid = p;
    deadChildStatus = status;
  }
}

/*********************
 * redirectIn(): performs input redirection 
 *********************/
void redirectIn(char* file) {
  /*open a file*/
  int sourceFD = open(file, O_RDONLY);
  if (sourceFD == -1) { 
    printf("open() failed on %s\n",file); 
  }
  /*redirect input*/
  int result = dup2(sourceFD,0);
  if (result == -1) { 
    printf("dup2() failed on input file %s\n",file); 
  }
}

/*********************
 * redirectOut(): performs output redirection 
 *********************/
void redirectOut(char* file) {
  /*open a file*/
  int targetFD = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (targetFD == -1) { 
    printf("open() failed on %s\n",file); 
  }
  /*redirect output*/
  int result = dup2(targetFD,1);
  if (result == -1) { 
    printf("dup2() failed on output file %s\n",file); 
  }
}

/*********************
 * execute(): process commandline options and exec 
 *********************/
void execute(char* command,int* spawnExit) {
  /*bools to process commandline options */
  int background = 0;
  int redirectInBool = 0;
  int redirectOutBool = 0;

  /*necessary for strtok*/ 
  const char space[2] = " ";
  /*array to point to individual tokens*/
  char* arguments[MAX_ARGS + 1];
  int i;
  for (i = 0; i < MAX_ARGS + 1; i ++) {
    arguments[i] = NULL;
  }
  int argCount = 0;
 
  /*parse the command*/
      //tokenize command
  char* token = strtok(command,space);
  while (token != NULL) {
    if (strcmp(token,"<") == 0) {
      redirectInBool = 1 - redirectInBool;
      token = strtok(NULL,space);
    } else if (strcmp(token,">") == 0) {
      redirectOutBool = 1 - redirectOutBool;
      token = strtok(NULL,space);
    } else if (strcmp(token,"&") == 0) {
      background = 1 - background;
      token = strtok(NULL,space);
    } else {
      /*handle redirection and reset redirection flags*/
      if (redirectInBool) {
        redirectIn(&token);
        redirectInBool = 1 - redirectInBool;
      } else if (redirectOutBool) {
        redirectOut(&token);
        redirectOutBool = 1 - redirectOutBool;
      } else {
        /*add argument to exec argument list*/
        arguments[argCount] = token;
        argCount++;
        token = strtok(NULL,space);
      }
    }
  }
  /*fork the process*/
  pid_t spawnpid = -5;
  spawnpid = fork();
  switch (spawnpid) {
    case -1:
      printf("Fork error. Exiting.\n");
      fflush(stdout);
      exit(1);
      break;
    case 0:
      //reset sigint for the child
      signal(SIGINT,SIG_DFL);
      if (argCount < MAX_ARGS) { 
        execvp(arguments[0],arguments);
        exit(1);
      } else {
        printf("Max number of arguments exceeded. Try again.\n");
        fflush(stdout);
      }   
      break;
    default:
      if (toggleBackgroundProc == 1 && background == 1) {
        printf("Running child [%d] in background\n",spawnpid);
        fflush(stdout);
        spawnpid = waitpid(spawnpid,spawnExit,WNOHANG);
      } else {
        printf("Running child [%d] in foreground\n",spawnpid);
        fflush(stdout);
        spawnpid = waitpid(spawnpid,spawnExit,0);
        printf("Execute: Child process [%d] has terminated with exit status %d\n",(int)spawnpid,*spawnExit); 
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
  /*if there's nothing where directory should be
   * cd home */
  if (dir[dirStart] == '\0') {
      err = chdir(getenv("HOME"));
  } else {
      /*use second space-delim string; ignore everything else*/
      token = strtok(dir,space);
      token = strtok(NULL,space);
      err = chdir(token);
   }
  if (err == -1) {
    printf("Could not find that directory\n");
    fflush(stdout);
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
  } else {
    //run anything non-native
    execute(rawCmd,&exitStatus);
  }
  
  free(cmd);
  /*check for dead child*/
  pid_t lastDead = deadChildPid;
  if (deadChildPid != lastDead) {
    printf("Child process [%d] has terminated with exit status %d\n",(int)deadChildPid,deadChildStatus);
    lastDead = deadChildPid;
  }
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
  
  //handle child process termination
  struct sigaction sa_sigchld = {0};
  sa_sigchld.sa_handler = sigchldHandler; 
  sigfillset(&sa_sigchld.sa_mask);
  sa_sigchld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa_sigchld, NULL);
  
  pid_t lastDead = deadChildPid;
  //endless loop; exit will terminate shell from
  //processCmd function
  while (1 == 1) {
    /*check for dead child*/
    if (deadChildPid != lastDead) {
      printf("Child process [%d] has terminated with exit status %d\n",(int)deadChildPid,deadChildStatus);
      lastDead = deadChildPid;
    }
    /*get and process input*/
    char* command = 0;
    command = getInput(command); 
    /*skip all processing for comments and empty strings*/
    if (command != 0 && command[0] != '#' && command[0] != '\0' && command[0] != '\n') {
      processCmd(command); 
    }
    if (command != 0) {
      free(command);
    }
  }
}
