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
	dir = opendir(".");

	if(dir){
		printf("Here are the list of files:\n");
		printf("----------------------------------------------");	
		while((ent = readdir(dir)) != NULL){
			if(ent->d_type != 4){
				stat(ent->d_name, &file_stats);
				printf("\n%s (%lld bytes)", ent->d_name, (long long)file_stats.st_size);
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

	if(remove(input) == 0){
		printf("File deleted successfully!\n");
	}

	else {
		printf("(!!) ERROR: There was a problem with deleting the file. Please try again later.\n");
	}

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
	printf("LIST: List the names of the files currently stored in the server.\n");
	printf("UPLOAD: Upload a file to the server.\n");
	printf("DOWNLOAD: Download a file from the server.\n");
	printf("DELETE: Delete a file from the server.\n");
	printf("QUIT: Exit BitDrive.\n\n");
}

void input_option(){
	get_input();
	if(strcmp(input, "UPLOAD") == 0){
		free(input);
		upload();
	}
	
	else if(strcmp(input, "DOWNLOAD") == 0){
		free(input);
		download();
	}

	else if(strcmp(input, "LIST") == 0){
		free(input);
		list();
	}

	else if(strcmp(input, "DELETE") == 0){
		free(input);
		delete();
	}

	else if(strcmp(input, "QUIT") == 0){
		free(input);
		quit();
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

