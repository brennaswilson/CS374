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

int key_values[8000];
int text_values[8000];

void key_indexes(char sent_key[]){
  char key_letter;
  for(int i = 0; i < strlen(sent_key); i++){
    key_letter = sent_key[i];
    char *p = strchr(allowed_characters, key_letter);
    key_values[i] = p - allowed_characters;
  }
}

void text_indexes(char message[]){
  char letter;
  for(int i = 0; i < strlen(message); i++){
    letter = message[i];
    char *p = strchr(allowed_characters, letter);
    text_values[i] = p - allowed_characters;
  }
}


void encrypt(int key_values[], int text_values[], int length_text, char encrypted_message[]){
  int encrypted_indices[8000];
  int modulo_indices[8000];

  for(int i = 0; i < length_text; i++){
    int add_textkey = key_values[i] + text_values[i];
    encrypted_indices[i] = add_textkey;
  }

  for(int i = 0; i < length_text; i++){
    int modulo_textkey = encrypted_indices[i] % 27;
    modulo_indices[i]= modulo_textkey;
  }

  for(int i = 0; i < length_text; i++){
    int index = modulo_indices[i];
    encrypted_message[i] = allowed_characters[index];

  }

  encrypted_message[length_text] = '\0';
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead, keyRead;
  char plaintext_buffer[8000];
  char encryptkey_buffer[8000];
  char encrypted_message[8000];
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

    printf("SERVER: Connected to client running at host %d port %d\n", 
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));

    // Get the message from the client and display it
    memset(plaintext_buffer, '\0', sizeof(plaintext_buffer));

    // Read the client's text from the socket
    charsRead = recv(connectionSocket, plaintext_buffer, sizeof(plaintext_buffer) - 1, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }
    printf("SERVER: I received this from the client: \"%s\"\n", plaintext_buffer);

    // Read the client's key from the socket
    memset(encryptkey_buffer, '\0', sizeof(encryptkey_buffer));
    keyRead = recv(connectionSocket, encryptkey_buffer, sizeof(encryptkey_buffer)-1, 0); 
    if (keyRead < 0){
      error("ERROR reading from socket");
    }
    printf("SERVER: I received this from the client: \"%s\"\n", encryptkey_buffer);

    key_indexes(encryptkey_buffer);
    text_indexes(plaintext_buffer);
    int length_text = strlen(plaintext_buffer);
    encrypt(key_values, text_values, length_text, encrypted_message);
    
    printf("Encrypted message: %s\n", encrypted_message);

    // Send a Success message back to the client
    charsRead = send(connectionSocket, 
                    encrypted_message, strlen(encrypted_message), 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    
    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}
