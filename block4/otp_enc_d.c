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
  //vars for client-server sockets
  int listenSocketFD, establishedConnectionFD = -5, portNumber, charsRead;
  socklen_t sizeOfClientInfo;
  struct sockaddr_in serverAddress, clientAddress;
  char longbuffer[256];
  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); 
  }


  //fork 5 processes to handle requests
  pid_t spawnpid[MAX_FORKS];
  int i;
  int r;
  char tmpOut[256];
  memset(tmpOut,'\0',sizeof(tmpOut));
  char buffer[256];
  memset(tmpOut,'\0',sizeof(buffer));

  //IPC socket to share file descriptors 
  char unix_buffer[256];
  memset(unix_buffer,'\0',sizeof(unix_buffer));
  int pair[2], unixFD;
  if (socketpair(PF_UNIX,SOCK_DGRAM,0,pair) < 0) {
    error("socketpair failed in main\n");
  }
 
  //set up fifo to control access to unix socket
  char* fifoFilename = "myfifo";
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
  int newfifo = mkfifo(fifoFilename,fifo_mode);
  int fifoFD = -5;

  for (i = 0; i < MAX_FORKS; i++) {
    spawnpid[i] = fork();
    if (spawnpid[i] == -1) {
      error("Fork error. Exiting.\n");
      /*********************************************
       * CHILD PROCESSES
       * *******************************************/
    } else if (spawnpid[i] == 0) {
      close(pair[0]);
      unixFD = pair[1];
      while (1) {
       // unixFD = pair[0];
        //handler to wake up on sigcont
        signal(SIGCONT,handler);

        printf("child %d sleeping\n",getpid());
        fflush(stdout);

        pause();

        printf("child %d woke up\n",getpid());
        fflush(stdout);

        signal(SIGCONT,SIG_IGN);

        //read FD integer from parent through fifo
        //fifo used so only one process (the first to get to the fifo)
        //reads the FD data from the unix socket
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

        r = strstr(tmpOut,"@") - tmpOut;
        tmpOut[r] = '\0';

        printf("%d read %s from fifo as string\n",getpid(),tmpOut);
        fflush(stdout);

        //access file descriptor through unix socket
        struct msghdr msg;
        struct cmsghdr *cmsg;
        struct iovec iov;
        char dummy[100];
        char buff[CMSG_SPACE(sizeof(int))];
        size_t readlen;
        int* fdlist;

        //response data
        iov.iov_base = dummy;
        iov.iov_len = sizeof(dummy);
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_controllen = CMSG_SPACE(sizeof(int));
        msg.msg_control = buff;
        readlen = recvmsg(unixFD, &msg, 0);
        if ((int)readlen <= 0) {
          error("recvmsg in parent process\n");
        } 
        establishedConnectionFD = -1; 
        for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
          if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
            //nfds = (cm->cmsg_len - CMSG_LEN(0)) / sizeof(int);
            fdlist = (int*)CMSG_DATA(cmsg);
            establishedConnectionFD = *fdlist;
            break;
          }
        } 
        
        printf("trying to write to FD %d\n",establishedConnectionFD);
        fflush(stdout);

        //verify connection 
        if (establishedConnectionFD > 0) {
          printf("fd is %d\n",establishedConnectionFD);
          fflush(stdout);
          //get the message from the client and display it
          memset(longbuffer, '\0', 256);
          charsRead = recv(establishedConnectionFD, longbuffer, 255, 0); // Read the client's message from the socket
          if (charsRead < 0) {
            error("ERROR reading from socket in 147");
          }
          printf("SERVER %d: I received this from the client: \"%s\"\n", getpid(),longbuffer);

          //send a Success message back to the client
          charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
          if (charsRead < 0) {
            error("ERROR writing to socket");
          }
          close(establishedConnectionFD); // Close the existing socket which is connected to the client
          //do work
          exit(0);
        }
      }
    }
  }
  /*********************************************
   * PARENT PROCESS
   * *******************************************/
  close(pair[1]);
  unixFD = pair[0];
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
  if (bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
    error("ERROR on binding");
  }
  listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

  
  //vars for IPC msg
  struct msghdr msg;
  struct cmsghdr *cmsg;
  struct iovec iov;
  char buff[CMSG_SPACE(sizeof(int))];
  char dummy[2]; 
  
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

    //IPC message
    memset(&msg, 0, sizeof(msg));
    msg.msg_controllen = CMSG_SPACE(sizeof(int));
    msg.msg_control = &buff;
    memset(msg.msg_control,0,msg.msg_controllen);
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *(int *)CMSG_DATA(cmsg) = establishedConnectionFD;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    iov.iov_base=dummy;
    iov.iov_len=1;
    dummy[0] = 0;

    //send the FD
    if (sendmsg(unixFD, &msg, 0) < 0) {
      error("sendmsg failed\n");
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
    sprintf(tmpIn,"%d@",establishedConnectionFD); 

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
