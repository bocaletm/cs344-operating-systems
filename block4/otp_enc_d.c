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

//set up fifo to control access to unix socket
const char* fifoFilename = "encfifo";

const int MAX_FORKS = 5;
int caught_signal = -5;

//global for process to tell parent if it's free
int usr_interrupt = 0;

/************************
 * handler: delete fifo file
 * **********************/
void fifoHandler(int signo) {
  remove(fifoFilename);
  exit(0);
}

/************************
 * handler: broken pipes in child process 
 * **********************/
void pipeHandler(int signo) {
  char* message = "error broken pipe\n";
  write(STDERR_FILENO,message,18);
  fflush(stderr);
}

/************************
 * handler: do not exit on signal from child
 * **********************/
void handler(int signo) {
  caught_signal = signo;
}

/************************
 *  Error function used for reporting major issues
 *************************/
void errorx(const char *msg) { 
  perror(msg);
  exit(1); 
}

/************************
 *  Error function used for reporting minor issues
 *************************/
void error(const char *msg) { 
  fprintf(stderr,msg);
}

/************************
 *  encrypt(): encrypts param2 using param1 key
 *************************/
void encrypt(char* key, char* msg, int n) {
  int ASCII_A = 65;
  int SPC_ALIAS = 91;
  int i;
  int c1;
  int c2;
  for (i = 0; i < n; i++) {
    //handle spaces, treated as character after Z
    if (msg[i] == ' ') {
      msg[i] = SPC_ALIAS;
    }
    if (key[i] == ' ') {
      key[i] = SPC_ALIAS;
    }
    //convert char to int, starting with A = 0
    c1 = (int)msg[i] - ASCII_A; 
    c2 = (int)key[i] - ASCII_A;
    //add two chars
    c1 += c2;
    //modulus operation
    c1 = c1 % 27;
    //add 65 back
    c1 += ASCII_A;
    //rollover if calculation is out of range 
    if (c1 > SPC_ALIAS) {
      c1 -= 27;
      msg[i] = (char)c1;
    } else if (c1 < ASCII_A) {
      c1 += 27;
      msg[i] = (char)c1;
    } else {
      msg[i] = (char)c1;
    }
    //restore spaces
    if (msg[i] == '[') {
      msg[i] = ' ';
    }
  }
}

/************************
 * main()
 * *********************/
int main(int argc, char *argv[]) {
  //vars for client-server sockets
  int bufferSize = 100000;
  int listenSocketFD, establishedConnectionFD = -5, portNumber, charsRead;
  socklen_t sizeOfClientInfo;
  struct sockaddr_in serverAddress, clientAddress;
  char longbuffer[bufferSize];
  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); 
  }


  //fork 5 processes to handle requests
  pid_t spawnpid[MAX_FORKS];
  int i;
  int r;
  char tmpOut[bufferSize];
  memset(tmpOut,'\0',sizeof(tmpOut));
  char buffer[bufferSize];
  memset(tmpOut,'\0',sizeof(buffer));

  //IPC socket to share file descriptors 
  char unix_buffer[bufferSize];
  memset(unix_buffer,'\0',sizeof(unix_buffer));
  int pair[2], unixFD;
  if (socketpair(PF_UNIX,SOCK_DGRAM,0,pair) < 0) {
    errorx("socketpair failed in main\n");
  }

  //set up fifo to control access to unix socket
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
  int newfifo = mkfifo(fifoFilename,fifo_mode);
  int fifoFD = -5;


  for (i = 0; i < MAX_FORKS; i++) {
    spawnpid[i] = fork();
    if (spawnpid[i] == -1) {
      errorx("Fork error. Exiting.\n");
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
        //do not let broken pipe kill process
        signal(SIGPIPE,SIG_IGN);
        pause();
        //read FD integer from parent through fifo
        //fifo used so only one process (the first to get to the fifo)
        //reads the FD data from the unix socket
        fifoFD = open(fifoFilename,O_RDONLY);
        if (fifoFD <= 0) {
          continue;
        }
        while(strstr(tmpOut,"@") == NULL) {
          memset(buffer,'\0',sizeof(buffer));
          r = read(fifoFD,buffer,sizeof(buffer) - 1); 
          strcat(tmpOut,buffer);
          if (r <= 0) {
            break;
          }
        }
        if (tmpOut[0] == '\0') {
          continue;
        }

        r = strstr(tmpOut,"@") - tmpOut;
        tmpOut[r] = '\0';

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
          errorx("recvmsg in parent process\n");
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
        int msgSize = 2048;
        int sectionSize = 100000;
        char message[msgSize];
        memset(message,'\0',msgSize);

        int midx = 0, i;

        int authError = 0;
        if (establishedConnectionFD > 0) {
          memset(longbuffer, '\0', bufferSize);
          charsRead = recv(establishedConnectionFD, longbuffer, bufferSize - 1, 0); // Read the client's message from the socket
          if (charsRead < 0) {
            error("ERROR reading from socket\n");
            //basic authentication for otp_enc
          } else if (longbuffer[0] != 'e') {
            error("client failed to indentify itself in socket\n");
            authError = 1;
          }

          //skip the client's signature "e"
          for (i = 0; i < bufferSize; i++) {
            if (i > 0) {
              message[midx] = longbuffer[i];
              midx++;
            }
          }

          while(strstr(longbuffer,"@") == NULL) {
            charsRead = recv(establishedConnectionFD, longbuffer, bufferSize - 1, 0); // Read the client's message from the socket
            if (charsRead < 0) {
              error("ERROR reading from socket");
            }
            strcat(message,longbuffer);
          }

          char key[sectionSize];
          memset(key,'\0',sizeof(key));

          char text[sectionSize];
          memset(text,'\0',sizeof(text));
          //don't do any work on the message if the client failed to identify
          if (authError == 0) {
            //use the ; message divider to split key and plaintext
            int k = 0;
            int t = 0;
            i = 0;
            unsigned char c = message[i]; 
            int flip = 0;

            while (c != '@') {
              if (c == ';') {
                flip = 1;
                i++;
                c = message[i];
                continue;
              } else if (flip == 0) {
                key[k] = c;
                k++;
              } else if (flip == 1) {
                text[t] = c;
                t++;
              }
              i++;
              c = message[i];
            }

            if (k >= t) {
              encrypt(key,text,t);
              text[t] = '@';
            } else {
              error("The key is too short for the message\n");
              text[0] = '@';
              text[1] = '\0';
            }
          } else {
            text[0] = '@';
            text[1] = '\0';
          }
          //send decrypted message back to the client
          charsRead = send(establishedConnectionFD,text,strlen(text),0); 
          close(establishedConnectionFD); // Close the existing socket which is connected to the client
        }
      }
      exit(0);
    }
  }

  /*********************************************
   * PARENT PROCESS
   * *******************************************/
  //clean up fifo on termination
  signal(SIGINT,fifoHandler);
  signal(SIGTSTP,fifoHandler); 
  signal(SIGTERM,fifoHandler);

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
    errorx("ERROR opening socket");
  }

  // Enable the socket to begin listening
  if (bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
    errorx("ERROR on binding");
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
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    if (establishedConnectionFD < 0) {
      errorx("ERROR on accept");
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
      errorx("sendmsg failed\n");
    }

    //redirect work to an open child
    //wake up children
    for (i = 0; i < MAX_FORKS; i++) {
      kill(spawnpid[i],SIGCONT);
    }

    //write FD to fifo; process will hang until a child reads fifo 
    char tmpIn[256];
    memset(tmpIn,'\0',sizeof(tmpIn));
    sprintf(tmpIn,"%d@",establishedConnectionFD); 

    fifoFD = open(fifoFilename,O_WRONLY);
    if (fifoFD == -1) {
      errorx("fifo open error in parent\n");
    }
    write(fifoFD,tmpIn,strlen(tmpIn));
    close(fifoFD);
  }
  close(listenSocketFD); // Close the listening socket
  remove(fifoFilename);
  return 0;
}

