#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h> 
#include <unistd.h> 
#include <sys/stat.h>

struct File{
  char *filename;
  long long size; 
  struct File *next;
};

void start_server(int port);
void free_list(struct File *head);
struct File* create_list();
char *list_to_string(struct File *root);
int list(int clientfd);
int upload(int clientfd);
int download(int clientfd);
int delete(int clientfd);
int quit(int clientfd);
int invalid_input(int clientfd);
void process_request(char *request, int sockfd);
void display_welcome();
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

void start_server(int port){
  /* Initialization of Variables */
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  bool server_run;
  char *buffer;
  int sockfd, newsockfd, status;

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
    status = read(newsockfd, buffer, 255);
    if(status < 0) {
      error_occurred("ERROR reading from socket");
    }

    process_request(buffer, newsockfd);
  }

  close(newsockfd);
  close(sockfd);
  printf("Closing sockets...");
}

void process_request(char *request, int clientfd){
  int status;
  printf("Message: %s\n", request);
  if(strcmp(request, "LIST") == 0){
    status = list(clientfd);
  }

  else if(strcmp(request, "QUIT") == 0){
    status = quit(clientfd);
  }

  else {
    status = invalid_input(clientfd);
  }

  if(status < 0) {
    error_occurred("ERROR writing to socket");
  }
}

void display_welcome(){
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Welcome to BitDrive Server! \n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void set_sockaddr(struct sockaddr_in *socket_addr, int port){
  bzero((char *) socket_addr, sizeof(*socket_addr));
  socket_addr->sin_family = AF_INET;
  socket_addr->sin_addr.s_addr = INADDR_ANY;
  socket_addr->sin_port = port;
}

void error_occurred(const char *msg){
  perror(msg);
  exit(1);
}

int list(int clientfd){
  char *response = list_to_string(create_list());
  int status = write(clientfd, response, strlen(response));
  free(response);
  return status;
}

int upload(int clientfd){
  return 0;
}

int download(int clientfd){
  return 0;
}

int delete(int clientfd){
  return 0;
}

int quit(int clientfd){
  char *response = "Disconnecting...";
  return write(clientfd, response, strlen(response));
}

int invalid_input(int clientfd){
  char *response = "Invalid input. Please try again.";
  return write(clientfd, response, strlen(response));
}

void free_list(struct File* head){
  struct File *tmp;
  int counter = 0;
  while (head != NULL) {
    tmp = head;

    counter++;
    // printf("%d %s\n", counter, tmp->filename);

    head = head->next;
    free(tmp->filename);
    free(tmp);
    // printf("Freed temp.\n");
  }

  free(head);
}

struct File* create_list(){
  char path[] = "Files/";
  struct stat file_stats;
  struct File *root;
  root = (struct File *)malloc(sizeof(struct File));
  root->next = NULL;
  root->filename = NULL;

  struct File *current;
  current = root;

  DIR *dir;
  struct dirent *ent;
  dir = opendir(path);

  if(dir){
    while((ent = readdir(dir)) != NULL){
      if(ent->d_type == 8){ 
        if(current->filename != NULL){
          struct File *file;
          file = (struct File *)malloc(sizeof(struct File));
          current->next = file;
          current = file;
          current->next = NULL;
        }

        char *file_path = malloc(strlen(path) + strlen(ent->d_name) + 1);
        char *filename = malloc(strlen(ent->d_name) + 1);
        strcpy(filename, ent->d_name);
        strcpy(file_path, path);
        strcat(file_path, ent->d_name);
        stat(file_path, &file_stats);

        current->filename = filename;
        current->size = (long long)file_stats.st_size;
        free(file_path);
      }
    }
  }

  else {
    printf("There are no files available for download.\n");   
  }

  free(ent);
  free(dir);
  return root;
}

char *list_to_string(struct File *root){
  struct File *current;
  current = root;
  long long size = 0;
  int file_counter = 0;

  // get size for malloc
  while (current != NULL){
    size += sizeof(*current);
    file_counter++;
    current = current->next;
  }

  char *list_string = malloc(size + (2*file_counter) + file_counter);
  strcpy(list_string, "\0");
  current = root; 
  while (current != NULL){
    //convert current->size to string
    char file_size[20];
    sprintf(file_size, "%llu", current->size);
    
    strcat(list_string, current->filename);
    strcat(list_string, " (");
    strcat(list_string, file_size);
    strcat(list_string, " bytes)\n");
    current = current->next;
  }

  free_list(root);
  return list_string;
}

