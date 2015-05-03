#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void display_welcome();
void display_commands();
char get_command();
char *process_command(char command, int sockfd);
void set_sockaddr(struct sockaddr_in *socket_addr, int port);
void start_client(char *server, int port);
void error_occurred(const char *msg);

int main(int argc, char* argv[]){
  if(argc < 3) {
    printf("Usage: %s <ip of server> <port>", argv[0]);
    exit(0);
  }

  display_welcome();
  start_client(argv[1], atoi(argv[2]));
  return 0;
}

void start_client(char *server, int port){
  struct sockaddr_in server_addr;
  struct hostent *host_addr;
  int sockfd, status, n;
  char command;
  char *response;
  bool is_running;

  set_sockaddr(&server_addr, htons(port));

  host_addr = gethostbyname(server);
  if(host_addr == NULL){
    error_occurred("ERROR, no such host\n");
    exit(0);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    error_occurred("ERROR opening socket");
  }

  bcopy((char *)host_addr->h_addr,
        (char *)&server_addr.sin_addr.s_addr,
        host_addr->h_length);

  status = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(status < 0){
    error_occurred("ERROR connecting");
  }

  printf("Connected to: %s\n", server);

  is_running = true;
  while(is_running){
    command = get_command();
    response = process_command(command, sockfd);
    printf("%s\n", response);

    if(strcmp(response, "Disconnecting...") == 0){
    	printf("Thank you for using BitDrive!\n");
    	is_running = false;
    }

    free(response);
  }

  close(sockfd);
}

char *process_command(char command, int sockfd){
  //process the command
  //returns the response from the server
  char *buffer;
  char *response;
  int status;

  response = malloc(sizeof(char) * 256);
  bzero(response, 256);

  switch(command){
    case 'L':
      buffer = "LIST";
      break;
    case 'U':
    	buffer = "UPLOAD";
    	break;
    case 'D':
    	buffer = "DOWNLOAD";
    	break;
    case 'X':
    	buffer = "DELETE";
    	break;
    case 'Q':
    	buffer = "QUIT";
    	break;
    default:
      break;
  }

  status = write(sockfd, buffer, strlen(buffer)); //send command
  if(status < 0) {
    error_occurred("ERROR writing to socket");
  }

  status = read(sockfd, response, 255); //receive the response
  if(status < 0){
    error_occurred("ERROR reading from socket");
  }

  return response;
}

void display_welcome(){
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Welcome to BitDrive! \n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void display_commands(){
	printf("\nWhat would you like to do?\n");
	printf("==================================================================\n");	
	printf("[L] LIST: List the names of the files currently stored in the server.\n");
	printf("[U] UPLOAD: Upload a file to the server.\n");
	printf("[D] DOWNLOAD: Download a file from the server.\n");
	printf("[X] DELETE: Delete a file from the server.\n");
	printf("[Q] QUIT: Exit BitDrive.\n\n");
}

char get_command(){
  char command;
  display_commands();
  //get the actual command here
  //limit it to one character
  //return the chosen character
  //Note: U is unknown; error option

	printf("> ");
	scanf("%s", &command);
  return command;
}

void set_sockaddr(struct sockaddr_in *socket_addr, int port) {
  bzero((char *) socket_addr, sizeof(*socket_addr));
  socket_addr->sin_family = AF_INET;
  socket_addr->sin_port = port;
}

void error_occurred(const char *msg){
  perror(msg);
  exit(0);
}
