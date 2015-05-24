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
char *get_input();
void send_request(int sockfd, char *buffer);
char *recv_response(int sockfd, char *response);
char *process_command(char command, int sockfd);
void set_sockaddr(struct sockaddr_in *socket_addr, int port);
void list(char *response);
void delete(int sockfd, char *response);
void download(int sockfd, char *response);
char *send_filename(char *filename, char *response);
void upload(int sockfd, char *response);
bool send_file(int sockfd, char *filename);
bool recv_file(int clientfd, char *filename);
bool send_command(char command, int sockfd);

void run_bitdrive(int sockfd);
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

  run_bitdrive(sockfd);
  close(sockfd);
}

void set_sockaddr(struct sockaddr_in *socket_addr, int port) {
  bzero((char *) socket_addr, sizeof(*socket_addr));
  socket_addr->sin_family = AF_INET;
  socket_addr->sin_port = port;
}

char get_command(){
  char command;
  printf("> ");
  scanf("%s", &command);
  return command;
}

char *get_input(){
  printf("> ");
  char *input = malloc(sizeof(char) * 256);
  scanf("%s", input);
  return input;
}

void send_request(int sockfd, char *buffer){
  int status = write(sockfd, buffer, strlen(buffer));
  if(status < 0) {
    error_occurred("ERROR writing to socket");
  }  
}

char *recv_response(int sockfd, char *response){
  int status = read(sockfd, response, 255); //receive the response
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
  printf("\nCommands:\n");
  printf("==================================================================\n");  
  printf("[L] LIST: List the names of the files currently stored in the server.\n");
  printf("[U] UPLOAD: Upload a file to the server.\n");
  printf("[D] DOWNLOAD: Download a file from the server.\n");
  printf("[X] DELETE: Delete a file from the server.\n");
  printf("[V] VIEW: View list of commands.\n");
  printf("[Q] QUIT: Exit BitDrive.\n\n");
}

void error_occurred(const char *msg){
  perror(msg);
  exit(0);
}

void run_bitdrive(int sockfd){
  bool is_running = true;
  char command;
  display_commands();

  while(is_running){
    printf("\nWhat would you like to do?\n");
    command = get_command();
    is_running = send_command(command, sockfd);
  }    

  return;
}

char *process_command(char command, int sockfd){
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

  if(command != 'V'){
    send_request(sockfd, buffer);
    return recv_response(sockfd, response);    
  }

  return NULL;
}

bool send_command(char command, int sockfd){
  char *response;
  response = process_command(command, sockfd);

  switch(command){
    case 'L':
      list(response);
      break;
    case 'U':
      upload(sockfd, response);
      break;
    case 'D':
      download(sockfd, response);
      break;
    case 'X':
      delete(sockfd, response);
      break;
    case 'V':
      display_commands();
      break;
    case 'Q':
      printf("%s\n", response);
      break;
  }  

  if(command == 'Q' && strcmp(response, "Disconnecting...") == 0){
    free(response);
    printf("Thank you for using BitDrive!\n");
    return false;
  }

  free(response);
  return true;
}

bool send_file(int sockfd, char *filename){
  FILE *file;
  file = fopen(filename, "rb");
  
  int bytes_read = 0;
  char *buffer = malloc(sizeof(char) * 256);
  bzero(buffer, 256);

  if(file == NULL){
    error_occurred("Error opening file.\n");
  }

  fseek(file, 0L, SEEK_END);
  int size = ftell(file);
  sprintf(buffer, "%d", size);
  write(sockfd, buffer, 256);
  fseek(file, 0L, SEEK_SET);
  while(true){
    bzero(buffer, 256);
    bytes_read = fread(buffer, 1, 256, file);

    if(bytes_read > 0){
      write(sockfd, buffer, bytes_read);
    }

    if(bytes_read < 256){
      if(feof(file)){
        printf("Upload done!\n");
        return true;
      }

      if(ferror(file)){
        printf("Error uploading file.\n");
        return false;
      }
    }
  }

  free(buffer);
  return false;
}

bool recv_file(int sockfd, char *filename){
  FILE *file;
  file = fopen(filename, "w+");
  
  int bytes_received = 0;
  char *buffer;
  buffer = malloc(sizeof(char) * 256);
  bzero(buffer, 256);

  if(file == NULL){
    error_occurred("Error opening file.\n");
  }

  read(sockfd, buffer, 256);
  int size = atoi(buffer);
  int sum_bytes_received = 0;
  int remaining_bytes = 0;

  while(sum_bytes_received < size) {
    bzero(buffer, 256);
    sum_bytes_received += bytes_received;
    remaining_bytes = size - sum_bytes_received;

    if(remaining_bytes < 256){
      bytes_received = read(sockfd, buffer, remaining_bytes);
    }

    else {      
      bytes_received = read(sockfd, buffer, 256);
    }

    if(bytes_received > 0){
      fwrite(buffer, 1, bytes_received, file);
    }
  
    if(bytes_received < 0){
      printf("Error reading file.");
      break;
    }

    if(bytes_received == 0){
      break;
    }
  }

  int status = fclose(file);
  if(status == 0) {
    printf("File received!\n");
    return true;
  }

  else {
    printf("ERROR: File not closed.\n");
  }

  return false;
}

void list(char *response){
  printf("Here are the list of files:\n");
  printf("----------------------------------------------\n"); 
  printf("%s", response);
  printf("----------------------------------------------\n");  
}

void delete(int sockfd, char *response){
  if(strcmp(response, "ready_delete") == 0){
    printf("Which file would you like to delete?\n");
    char *file = get_input();
    send_request(sockfd, file);
    free(file);

    bzero(response, 256);
    recv_response(sockfd, response);
    if(strcmp(response, "delete_success") == 0){
      printf("File deleted successfully!\n");
    }

    else{
      printf("There was an error deleting the file. Please try again.\n");
    }
  }
}

void upload(int sockfd, char *response){
  if(strcmp(response, "ready_upload") == 0){
    printf("What file do you want to upload?\n");
    char *filename = get_input();
    send_request(sockfd, filename);

    char path[] = "client_files/";
    strcat(path, filename);

    bzero(response, 256);
    recv_response(sockfd, response);

    if(strcmp(response, "ready_filename") == 0){
      if(send_file(sockfd, path) == false){
        printf("File not uploaded.\n");
      }
    }

    else {
      printf("Server is not yet ready. Try again.\n");
    }
  }

  else {
    printf("Server is not yet ready. Try again.\n");
  }
}

void download(int sockfd, char *response){  
  char path[] = "client_files/";
  if(strcmp(response, "ready_download") == 0){
    printf("What file do you want to download?\n");
    char *filename = get_input();
    send_request(sockfd, filename);
    strcat(path, filename);

    bzero(response, 256);
    recv_response(sockfd, response);
    if(strcmp(response, "ready_to_send") == 0){
      if(recv_file(sockfd, path) == false){
        printf("File not downloaded. Try again\n");
      }
    }

    else {
      printf("Server not yet ready. Try again.\n");
    }
  }

  else {
    printf("Server is not yet ready. Try again.\n");
  }
}
