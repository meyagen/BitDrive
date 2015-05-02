#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void display_welcome();
void start_server(int port);
void set_sockaddr(struct sockaddr_in *socket_addr, int port);
void error_occurred(const char *msg);

int main(int argc, char* argv[]) {
  if(argc < 2) {
    error_occurred("No port was provided");
  }

  display_welcome();
  start_server(atoi(argv[1]));
  return 0;
}

void display_welcome() {
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Welcome to BitDrive Server! \n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void start_server(int port) {
  /* Initialization of Variables */
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  bool server_run;
  char *buffer;
  int sockfd, newsockfd, status, n;

  /* Initial Values */
  set_sockaddr(&server_addr, htons(port));
  client_len = sizeof(client_addr);
  server_run = true;
  buffer = malloc(sizeof(char) * 256);
  bzero(buffer, 256);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    error_occurred("ERROR opening socket");
  }

  status = bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(status < 0) {
    error("ERROR on binding");
  }

  listen(sockfd, 5);
  printf("Server has started.\n");
  printf("Now listening to port: %d \n", port);

  /* Waiting for client to connect */
  newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
  if (newsockfd < 0) {
    error_occurred("ERROR on accept");
  }

  /* Main Loop */
  while(server_run) {
    n = read(newsockfd, buffer, 255);
    if(n < 0) {
      error_occurred("ERROR reading from socket");
    }

    printf("Here is the message: %s", buffer);

    n = write(newsockfd, "I got your message", 18);
    if(n < 0) {
      error_occurred("ERROR writing to socket");
    }
  }

  close(newsockfd);
  close(sockfd);
  printf("Closing sockets...");
}

void set_sockaddr(struct sockaddr_in *socket_addr, int port) {
  bzero((char *) socket_addr, sizeof(*socket_addr));
  socket_addr->sin_family = AF_INET;
  socket_addr->sin_addr.s_addr = INADDR_ANY;
  socket_addr->sin_port = port;
}

void error_occurred(const char *msg) {
  perror(msg);
  exit(1);
}
