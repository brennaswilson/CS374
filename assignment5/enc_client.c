#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[8000];
  
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"Error: please provide enc_client, plaintext, keyfile, portnumber\n"); 
    exit(0); 
  } 

  //check plaintext for bad characters
  FILE *file = fopen(argv[1], "r");
  char characters; 
  const char allowed_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ \n";

  while((characters = fgetc(file)) != EOF){
    if(strchr(allowed_characters, characters) == NULL){
        fprintf(stderr,"error: input contains bad characters\n"); 
        fclose(file);
        exit(1); 
    }
  }

  // check key for length 
  characters = '\0';
  int number_characters = 0;
  rewind(file);
  FILE *keyfile = fopen(argv[2], "r");
  int key_length = 0;

  while ((characters = fgetc(file)) != EOF){
    number_characters++;
  }

  while ((characters = fgetc(keyfile)) != EOF){
    key_length++;
  }

  if (number_characters > key_length ){
    fprintf(stderr,"Error: key %s is too short\n", argv[2]); 
    fclose(keyfile);
    exit(1); 
  }
  rewind(file);
  rewind(keyfile);

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }

  size_t ret = fread(buffer, 1, sizeof(buffer) - 1, file);
  // Remove the trailing \n that fgets adds
  buffer[ret++] = '\0';
  fclose(file);
  fclose(keyfile);

  // Send message to server
  // Write to the server
  charsWritten = send(socketFD, buffer, ret, 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < ret){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  // Get return message from server
  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));
  // Read data from the socket, leaving \0 at end
  charsRead = recv(socketFD, buffer, sizeof(buffer), 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

  // Close the socket
  close(socketFD); 
  return 0;
}