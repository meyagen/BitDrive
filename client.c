#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void display_welcome();
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

void display_welcome(){
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Welcome to BitDrive! \n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void start_client(char *server, int port){
  struct sockaddr_in server_addr;
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  struct hostent *host_addr;
  int sockfd, status, n;

  char *buffer = malloc(sizeof(char) * 256);
  bzero(buffer, 256);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    error_occurred("ERROR opening socket");
  }

  host_addr = gethostbyname(server);
  if(host_addr == NULL){
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  bcopy((char *)host_addr->h_addr,
        (char *)&server_addr.sin_addr.s_addr,
        host_addr->h_length);

  status = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(status < 0){
      error_occurred("ERROR connecting");
  }

  printf("Please enter the message: ");
  fgets(buffer, 255, stdin);

  n = write(sockfd, buffer, strlen(buffer));
  if(n < 0) {
    error_occurred("ERROR writing to socket");
  }

  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  if(n < 0){
    error_occurred("ERROR reading from socket");
  }

  printf("%s\n", buffer);
  close(sockfd);
}

void error_occurred(const char *msg){
    perror(msg);
    exit(0);
}
