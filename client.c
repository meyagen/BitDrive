#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h> 
#include <sys/stat.h>

typedef enum {LIST, UPLOAD, DOWNLOAD, DELETE, QUIT} server_options;
server_options option;
struct stat file_stats;

char path[] = "Files/";
char *input;
bool loop = true;

void get_input(){
	printf("> ");
	input = malloc(20*sizeof(char));
	scanf("%s", input);	
}

void list(){
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path);

	if(dir){
		printf("Here are the list of files:\n");
		printf("----------------------------------------------");	
		while((ent = readdir(dir)) != NULL){
			if(ent->d_type != 4){	
				char *file_path = malloc(strlen(path) + strlen(ent->d_name) + 1);
				char *filename = malloc(strlen(ent->d_name) + 1);
				strcpy(filename, ent->d_name);
				strcpy(file_path, path);
				strcat(file_path, filename);

				stat(file_path, &file_stats);
				printf("\n%s (%lld bytes)", filename, (long long)file_stats.st_size);
				free(filename);
				free(file_path);
			}
		}
		printf("\n----------------------------------------------\n\n");	
	}

	else {
		printf("There are no files available for download.\n");		
	}

	free(dir);
}

int upload(){
	printf("What file would you like to upload?\n");
	get_input();

	free(input);
	return 0;
}

int download(){
	printf("What file would you like to download?\n");
	get_input();

	free(input);
	return 0;
}

int delete(){
	printf("What file would you like to delete?\n");
	get_input();

	char *file_path = malloc(strlen(path) + strlen(input) + 1);
	strcpy(file_path, path);
	strcat(file_path, input);

	if(remove(file_path) == 0){
		printf("File deleted successfully!\n");
	}

	else {
		printf("(!!) ERROR: There was a problem with deleting the file. Please try again later.\n");
	}

	free(file_path);
	free(input);
	return 0;
}

void quit(){
	printf("Thank you for using BitDrive!\n");
	loop = false;
}

void print_options(){
	printf("\nWhat would you like to do?\n");
	printf("==================================================================\n");	
	printf("[L] LIST: List the names of the files currently stored in the server.\n");
	printf("[U] UPLOAD: Upload a file to the server.\n");
	printf("[D] DOWNLOAD: Download a file from the server.\n");
	printf("[X] DELETE: Delete a file from the server.\n");
	printf("[Q] QUIT: Exit BitDrive.\n\n");
}

void input_option(){
	get_input();
	if(strcmp(input, "U") == 0){
		free(input);
		upload();
	}
	
	else if(strcmp(input, "D") == 0){
		free(input);
		download();
	}

	else if(strcmp(input, "L") == 0){
		free(input);
		list();
	}

	else if(strcmp(input, "X") == 0){
		free(input);
		delete();
	}

	else if(strcmp(input, "Q") == 0){
		free(input);
		quit();
	}

	else {
		free(input);
		printf("Invalid input. Please try again.\n");
	}
}

int main(){
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
	printf("Welcome to BitDrive! \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	

	while(loop == true){
		print_options();
		input_option();
	}

	return 0;
}

