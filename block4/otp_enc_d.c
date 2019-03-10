/*********************
 * Mario Bocaletti
 * cs344
 * otp_enc_d: server that encodes text
 *********************/
//#define _POSIX_SOURCE
//for signals
#include <signal.h>
//for general stuff
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//for sockets
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
//for forking
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

const int MAX_FORKS = 5;
int caught_signal = -5;

//global for process to tell parent if it's free
int usr_interrupt = 0;

/************************
 * handler
 * **********************/
void handler(int signo) {
  caught_signal = signo;
}

/************************
 *  Error function used for reporting issues
 *************************/
void error(const char *msg) { 
  perror(msg);
  fflush(stdout);
  exit(1); 
}

/************************
 * main()
 * *********************/
int main(int argc, char *argv[]) {
  //vars for sockets
  int listenSocketFD, establishedConnectionFD = -5, portNumber, charsRead;
  socklen_t sizeOfClientInfo;
  struct sockaddr_in serverAddress, clientAddress;
  char longbuffer[256];
  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); 
  }

  // Set up the address struct for this process (the server)
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

  // Set up the socket
  listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (listenSocketFD < 0) {
    error("ERROR opening socket");
  }

  // Enable the socket to begin listening
  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    error("ERROR on binding");
  }
  listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
  //set up fifo to share file descriptors
  char* fifoFilename = "myfifo";
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
  int newfifo = mkfifo(fifoFilename,fifo_mode);
  int fifoFD = -5;

  //fork 5 processes to handle requests
  pid_t spawnpid[MAX_FORKS];
  int i;
  int r;
  char tmpOut[256];
  memset(tmpOut,'\0',sizeof(tmpOut));
  char buffer[256];
  memset(tmpOut,'\0',sizeof(buffer));

  for (i = 0; i < MAX_FORKS; i++) {
    spawnpid[i] = fork();
    if (spawnpid[i] == -1) {
      error("Fork error. Exiting.\n");
    } else if (spawnpid[i] == 0) {
      while (1) {
        int* filePtr = 0;

        //handler to wake up on sigcont
        signal(SIGCONT,handler);

        printf("child %d sleeping\n",getpid());
        fflush(stdout);

        pause();

        printf("child %d woke up\n",getpid());
        fflush(stdout);

        signal(SIGCONT,SIG_IGN);

        //if fifo returns 0, another process has taken job
        fifoFD = open(fifoFilename,O_RDONLY);
        if (fifoFD <= 0) {
          continue;
        }
        printf("getting ready to read fifo\n");
        fflush(stdout);
        while(strstr(tmpOut,"@") == NULL) {
          memset(buffer,'\0',sizeof(buffer));
          r = read(fifoFD,buffer,sizeof(buffer) - 1); 
          strcat(tmpOut,buffer);
          printf("read fifo: [%s]\n",tmpOut);
          fflush(stdout);
          if (r <= 0) {
            break;
          }
        }
        if (tmpOut[0] == '\0') {
          continue;
        }

        printf("read %s from fifo as string\n",tmpOut);
        fflush(stdout);

        r = strstr(tmpOut,"@") - tmpOut;
        tmpOut[r] = '\0';
        establishedConnectionFD = atoi(tmpOut);

char fd_path[64];  // actual maximal length: 37 for 64bit systems
snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%d", SOURCE_PID, SOURCE_FD);
int new_fd = open(fd_path, O_RDWR);

        printf("trying to write to FD %d\n",establishedConnectionFD);
        //verify connection 
        if (establishedConnectionFD > 0) {
          //get the message from the client and display it
          memset(longbuffer, '\0', 256);
          charsRead = recv(*filePtr, longbuffer, 255, 0); // Read the client's message from the socket
          if (charsRead < 0) {
            error("ERROR reading from socket in 147");
          }
          printf("SERVER %d: I received this from the client: \"%s\"\n", getpid(),longbuffer);

          //send a Success message back to the client
          charsRead = send(*filePtr, "I am the server, and I got your message", 39, 0); // Send success back
          if (charsRead < 0) {
            error("ERROR writing to socket");
          }
          close(*filePtr); // Close the existing socket which is connected to the client

          //do work
          exit(0);
        }
      }
    }
  }

  //accept connections
  while (1) {
    printf("this is parent process %d\n",getpid());
    fflush(stdout);
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) {
      error("ERROR on accept");
    }

    printf("\naccepted connection at %d\n",establishedConnectionFD);
    fflush(stdout);

    //redirect work to an open child
    //wake up children
    printf("trying to wake up children from parent %d\n",getpid());
    fflush(stdout);
    for (i = 0; i < MAX_FORKS; i++) {
      kill(spawnpid[i],SIGCONT);
    }

    //write FD to fifo; process will hang until a child reads fifo 
    char tmpIn[256];
    memset(tmpIn,'\0',sizeof(tmpIn));
    sprintf(tmpIn,"%ls@",&establishedConnectionFD); 

    printf("writing %s to fifo as string\n",tmpIn);
    fflush(stdout);

    fifoFD = open(fifoFilename,O_WRONLY);
    if (fifoFD == -1) {
      error("fifo open error in parent\n");
    }
    write(fifoFD,tmpIn,strlen(tmpIn));
    close(fifoFD);
  }
  close(listenSocketFD); // Close the listening socket
  remove(fifoFilename);
  return 0;
}

