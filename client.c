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
void delete(int sockfd, char *response);
bool send_command(char command, int sockfd);

bool assert_equals(char *actual, char *expected);
void test_list(char *response);
void test_upload(char *response);
void test_download(char *response);
void test_delete(int sockfd, char *response);
void test_quit(char *response);
void test_commands(char command, int sockfd);
void test(int sockfd);

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

  // run_bitdrive(sockfd);
  test(sockfd);
  close(sockfd);
}

void set_sockaddr(struct sockaddr_in *socket_addr, int port) {
  bzero((char *) socket_addr, sizeof(*socket_addr));
  socket_addr->sin_family = AF_INET;
  socket_addr->sin_port = port;
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

  send_request(sockfd, buffer);
  return recv_response(sockfd, response);
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
  printf("\nWhat would you like to do?\n");
  printf("==================================================================\n");  
  printf("[L] LIST: List the names of the files currently stored in the server.\n");
  printf("[U] UPLOAD: Upload a file to the server.\n");
  printf("[D] DOWNLOAD: Download a file from the server.\n");
  printf("[X] DELETE: Delete a file from the server.\n");
  printf("[Q] QUIT: Exit BitDrive.\n\n");
}

bool send_command(char command, int sockfd){
  char *response;
  response = process_command(command, sockfd);

  switch(command){
    case 'L':
      printf("Here are the list of files:\n");
      printf("----------------------------------------------\n"); 
      printf("%s", response);
      printf("----------------------------------------------\n\n");
      break;
    case 'U':
      break;
    case 'D':
      break;
    case 'X':
      printf("%s", response);
      delete(sockfd, response);
      break;
    case 'Q':
      printf("%s\n", response);
      break;
    default:
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

void error_occurred(const char *msg){
  perror(msg);
  exit(0);
}

void run_bitdrive(int sockfd){
  bool is_running = true;
  char command;

  while(is_running){
    display_commands();
    command = get_command();
    is_running = send_command(command, sockfd);
  }

  return;
}

void delete(int sockfd, char *response){
  if(strcmp(response, "Which file would you like to delete?\n") == 0){
    char *file = get_input();
    send_request(sockfd, file);
    free(file);

    bzero(response, 256);
    recv_response(sockfd, response);
    printf("%s", response);
  }
}

void test_commands(char command, int sockfd){
  char *response;
  response = process_command(command, sockfd);

  switch(command){
    case 'L':
      printf("Here are the list of files:\n");
      printf("----------------------------------------------\n"); 
      printf("%s", response);
      printf("----------------------------------------------\n\n");
      test_list(response);
      break;
    case 'U':
      test_upload(response);
      break;
    case 'D':
      test_download(response);
      break;
    case 'X':
      printf("%s", response);
      test_delete(sockfd, response);
      break;
    case 'Q':
      printf("%s\n", response);
      test_quit(response);
      break;
    default:
      break;
  }  

  free(response);
}

void test_upload(char *response){  
}

void test_download(char *response){  
}

void test_delete(int sockfd, char *response){
  if(assert_equals(response, "Which file would you like to delete?\n")){
    send_request(sockfd, "lorem.txt");

    bzero(response, 256);
    recv_response(sockfd, response);
    printf("%s", response);

    if(assert_equals(response, "File deleted successfully!\n")){
      bzero(response, 256);
      char *list_response = process_command('L', sockfd);
      printf("Files remaining:\n%s", list_response);
      if(assert_equals(list_response, "readme.txt (44 bytes)\n")){
        printf("TEST: delete()\t PASS\n");      
      }

      else{
        printf("TEST: delete()\t FAIL\t File not deleted.\n");      
      }

      free(list_response);
    }

    else{
      printf("TEST: delete()\t FAIL\t File not deleted.\n");      
    }  
  }

  else{
    printf("TEST: delete()\t FAIL\t Incorrect response (should ask for file).\n");
  }
}

void test_list(char *response){
  if(assert_equals(response, "readme.txt (44 bytes)\nlorem.txt (244 bytes)\n")){
    printf("TEST: list()\t PASS\n");
  }

  else{
    printf("TEST: list()\t FAIL\n");
  }
}

void test_quit(char *response){
  if(assert_equals(response, "Disconnecting...")){
    printf("Thank you for using BitDrive!\n");
    printf("TEST: quit()\t PASS\n");
  }

  else{
    printf("TEST: quit()\t FAIL\n");        
  }
}

bool assert_equals(char *actual, char *expected){
  if(strcmp(actual, expected) == 0){
    return true;
  }

  return false;
}

void test(int sockfd){
  int i;
  char commands[3] = {'L', 'X', 'Q'};

  for(i = 0; i < 3; i++){
    test_commands(commands[i], sockfd);
  }

  return;
}
