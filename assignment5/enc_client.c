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
char *text_buffer;

int serve_send(int socket, FILE *file, int message_length) {
  int charsWritten;
  int charsChunk;
  int loopover = 1;
  size_t bytes_read;

  // send message size so server knows what to expect overall
  int buffer_size = message_length; 

  charsWritten = send(socket, &buffer_size, sizeof(buffer_size), 0); 
  // if server did not get anything, exit 
  if (charsWritten < 0) {
      fprintf(stderr,"ERROR writing buffer size to socket\n"); 
      exit(0);
  }

  // send message in chunks 
  while (loopover) {
      // Read up to 4000 bytes at a time from the file
      char *buffer = (char *)malloc(4000);
      bytes_read = fread(buffer, 1, 4000, file);
      if (bytes_read == 0) {
          // If no data was read or reached EOF, break the loop
          loopover = 0;
          break;
      }

      // Send the buffer over the socket
      if (bytes_read != 0){
        charsWritten = send(socket, buffer, bytes_read, 0);
        if (charsWritten < 0) {
          fprintf(stderr,"ERROR writing to socket\n"); 
          exit(0);

        }
      }
    free(buffer);
  }

  return 0; 
}

char* serve_receive(int socket) {
  int charsRead;
  int loopover = 1;
  size_t bytes_read;
  size_t bytes_received;
  int total_received = 0;
  int message_length;

  // receive total message size 
  charsRead = recv(socket, &message_length, sizeof(message_length), 0); 

  // if server did not get anything, exit 
  if (charsRead < 0) {
      fprintf(stderr,"ERROR reading from socket\n"); 
      exit(0);
  }
  // printf("CLIENT: Incoming message will be a total of: %d\n", message_length);


  char *buffer = malloc(message_length);
  int total_remaining = message_length;

  // receive message in chunks
  while (total_received < message_length) {

      // Read up to 4000 bytes at a time from the file, could be less if we are at the end of the file 

      if (total_remaining >= 4000){
        bytes_read = recv(socket, buffer + total_received, 4000, 0);
      }
      else{
        bytes_read = recv(socket, buffer + total_received, total_remaining, 0);
      }
      
      if (bytes_read < 0 ) {
        fprintf(stderr,"ERROR reading from socket\n"); 
        exit(0);
      }

      total_received += bytes_read;
      total_remaining -= bytes_read;

      }
      return buffer; 
  }


int main(int argc, char *argv[]) {
  int socketFD, charsWritten, charsRead, keyWritten;
  struct sockaddr_in serverAddress;

  
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"Error: please provide enc_client, plaintext, keyfile, portnumber\n"); 
    exit(0); 
  } 

  //check plaintext for bad characters
  FILE *text_file = fopen(argv[1], "r");
  char characters; 
  const char allowed_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ \n";

  while((characters = fgetc(text_file)) != EOF){
    if(strchr(allowed_characters, characters) == NULL){
        fprintf(stderr,"enc_client error: input contains bad characters\n"); 
        fclose(text_file);
        exit(1); 
    }
    
  }

  // check key for length 
  characters = '\0';
  int number_characters = 0;
  rewind(text_file);
  FILE *keyfile = fopen(argv[2], "r");
  int key_length = 0;

  while ((characters = fgetc(text_file)) != EOF){
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
  rewind(text_file);
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
    fprintf(stderr,"Error: could not contact enc_server on port %s\n", argv[3]); 
    exit(1);
  }

  char *client_type = "enc_client";
  charsWritten = send(socketFD, client_type, strlen(client_type), 0);
  if (charsWritten < 0) {
      fprintf(stderr, "ERROR writing to socket\n");
      exit(2);
  }

  char server_type[11];
  charsRead = recv(socketFD, server_type, sizeof(server_type) - 1, 0);
  if (charsRead < 0) {
      fprintf(stderr, "ERROR reading from socket\n");
      exit(2);
  }

  if (strcmp(server_type, "enc_server") != 0) {
      fprintf(stderr, "Error: could not contact enc_server on port%s\n", argv[3]);
      close(socketFD);
      exit(2);
  }

  // send plaintext 
  serve_send(socketFD, text_file, number_characters);

  //send key
  serve_send(socketFD, keyfile, key_length);

  fclose(text_file);
  fclose(keyfile);

  // Get return message from server
  // Read data from the socket, leaving \0 at end
  text_buffer = serve_receive(socketFD);

  int text_len = strlen(text_buffer);

  // printf("Text length is: %d\n", text_len);

  if (text_buffer < 0){
    error("CLIENT: ERROR reading from socket");
  }

  text_buffer[number_characters] = '\0';
  // printf("Text length is: %d\n", text_len);
  printf("%s", text_buffer);

  // // Close the socket
  close(socketFD); 
  return 0;
}