/*********************
 * Mario Bocaletti
 * cs344
 * otp_enc: client that encodes text
 *********************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define h_addr h_addr_list[0]   

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
 * validate():  
 *************************/
void validate(char* msg) {
  int i;
  int k = 0;
  int p = 0;
  //check for invalid characters (skip beginning and end markers)
  for (i = 1; i < strlen(msg) -1; i++) {
    if ((msg[i] < 65 || msg[i] > 90) && msg[i] != ' ' && msg[i] != ';') {
      errorx("otp_dec error: input contains bad characters\n");
    }
    if (msg[i] == ';') {
      k = i;
    }
  }
  //check for invalid key length
  p = i - k;
  if (k < p) {
    errorx("otp_enc error: the key is too short\n");
  }
}

/************************
 * readfile(): reads data from a file into a string 
 *************************/
void readfile(char* filename,char* target, int n) {
  FILE* file;
  int c;
  int idx = 0;
  file = fopen(filename,"r");
  if (file) {
    while ((c = getc(file)) != EOF) {
      //omit newlines
      if (c != '\n') {
        target[idx] = c;
        idx++;
      }
    }
  }
  if (idx == 0) {
    errorx("could not read file\n");
  } else if (idx >= n) {
    error("input text too long and was truncated\n");
  }

  //make sure it's null terminated
  target[n] = '\0';
  fclose(file);
}

int main(int argc, char *argv[]) {

  // Check usage & args
  if (argv[3] == NULL || argv[4] != NULL) { 
    errorx("otp_enc: wrong number of args\n"); 
  }

  //read files from args
  int fileLimit = 100000;
  int msgLimit = 200100;
  char* plaintext = 0;
  plaintext = malloc(fileLimit * sizeof(char));
  memset(plaintext,'\0',fileLimit);

  char* key = 0;
  key = malloc(fileLimit * sizeof(char));
  memset(key,'\0',fileLimit);

  char msg[msgLimit];
  memset(msg,'\0',msgLimit);

  readfile(argv[1],plaintext,fileLimit);
  readfile(argv[2],key,fileLimit);

  //compose msg including authentication prefix
  snprintf(msg,(size_t)msgLimit,"d%s;%s@",key,plaintext);
  
  free(plaintext);
  free(key);
  
  validate(msg);

  /********************************
   * SOCKET
   * *****************************/
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  char buffer[fileLimit];


  // Set up the server address struct
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
  if (serverHostInfo == NULL) { 
    errorx("CLIENT: ERROR, no such host\n"); 
  }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

  // Set up the socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (socketFD < 0) { 
    error("CLIENT: ERROR opening socket\n");
  }

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
    char errorMsg[50];
    memset(errorMsg,'\0',50);
    sprintf(errorMsg,"Error: could not contact otp_dec_d on port %d\n",portNumber);
    fprintf(stderr,errorMsg);
    fflush(stderr);
  }

  int totalSent = 0; 
  // Send message to server
  while (totalSent < strlen(msg)) {
    charsWritten = send(socketFD, msg, strlen(msg), 0); // Write to the server
    totalSent += charsWritten;
    if (charsWritten < 0) {
      error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(buffer)) {
      error("CLIENT: WARNING: Not all data written to socket!\n");
    }
  }

  // Get return message from server
  memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse

  while(strstr(buffer,"@") == NULL) {
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
  }

  if (charsRead < 0) {
    close(socketFD); // Close the socket
    errorx("CLIENT: ERROR reading from socket\n");
  }
  if (buffer[0] == '@') {
    close(socketFD); // Close the socket
    error("CLIENT: server could not encrypt message\n");
    exit(2);
  } else {
    //remove end of message and print
    buffer[strcspn(buffer, "@")] = '\0';
    printf("%s\n", buffer);
    fflush(stdout);
  }
  close(socketFD); // Close the socket
  return 0;
}
