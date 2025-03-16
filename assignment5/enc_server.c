#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

const char allowed_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ \n";

int text_indexes(char message[], int text_values[]){
  char letter;
  int len = strlen(message);
  for(int i = 0; i < len; i++){
    letter = message[i];
    char *p = strchr(allowed_characters, letter);
    text_values[i] = p - allowed_characters;
  }

}

int key_indexes(char sent_key[], int key_values[]){
  char key_letter;
  int len = strlen(sent_key);
  for(int i = 0; i < len; i++){
    key_letter = sent_key[i];
    char *p = strchr(allowed_characters, key_letter);
    key_values[i] = p - allowed_characters;
  }
}

void encrypt(int key_values[], int text_values[], char text_buffer[], char encrypted_message[]){
  int encrypted_indices[strlen(text_buffer)];
  int modulo_indices[strlen(text_buffer)];
  int length = strlen(text_buffer);

  for(int i = 0; i < length; i++){
    int add_textkey = key_values[i] + text_values[i];
    encrypted_indices[i] = add_textkey;
  }
  
  for(int i = 0; i < length; i++){
    int modulo_textkey = encrypted_indices[i] % 27;
    modulo_indices[i]= modulo_textkey;
  }

  for(int i = 0; i < length; i++){
    int index = modulo_indices[i];
    encrypted_message[i] = allowed_characters[index];
  }
  encrypted_message[length] = '\0';
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
  // printf("Incoming message will be a total of: %d\n", message_length);


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

  int serve_send(int socket, char encrypted_message[]) {
    int charsWritten;
    size_t bytes_sent;
    int total_sent = 0;
    int message_length = strlen(encrypted_message);
    int total_remaining = message_length;
    
    int buffer_size = message_length; 
    charsWritten = send(socket, &buffer_size, sizeof(buffer_size), 0); 

    // printf("Total size of array being sent: %d\n", message_length);

    // if server did not get anything, exit 
    if (charsWritten < 0) {
        fprintf(stderr,"ERROR writing buffer size to socket\n"); 
        exit(0);
    }

    while (total_sent < message_length) {

      // Read up to 4000 bytes at a time from the file, could be less if we are at the end of the file 

      if (total_remaining >= 4000){
        bytes_sent = send(socket, encrypted_message + total_sent, 4000, 0);
      }
      else{
        bytes_sent = send(socket, encrypted_message + total_sent, total_remaining, 0);
      }
      
      if (bytes_sent < 0 ) {
        fprintf(stderr,"ERROR reading to socket\n"); 
        exit(0);
      }
      total_sent += bytes_sent;
      total_remaining -= bytes_sent;

      }
      return 0;
    }
  

int main(int argc, char *argv[]){
  int connectionSocket, charsRead, keyRead, message_size, key_size, bytesReceived;
  char *text_buffer, *key_buffer, *encrypted_message;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    // printf("SERVER: Connected to client running at host %d port %d\n", 
    //                       ntohs(clientAddress.sin_addr.s_addr),
    //                       ntohs(clientAddress.sin_port));

    text_buffer = serve_receive(connectionSocket);
    // printf("Buffer is: %s\n", text_buffer);
    key_buffer = serve_receive(connectionSocket);
    // printf("Buffer is: %s\n", key_buffer);
    
    int text_values[strlen(text_buffer)];
    int key_values[strlen(key_buffer)];
    

    text_indexes(text_buffer, text_values);
    key_indexes(key_buffer, key_values);
    
    char encrypted_message[strlen(text_buffer)];
    encrypt(key_values, text_values, text_buffer, encrypted_message);
    
    serve_send(connectionSocket, encrypted_message);

    // printf("The encrypted message is: %s\n", encrypted_message);
    
    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}
