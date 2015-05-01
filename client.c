#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h> 
#include <sys/stat.h>


// Initialize variables and functions

struct stat file_stats;
struct File{
	char *filename;
	long long size;	
	struct File *next;
};

char path[] = "Files/";
char *input;
bool loop = true;

void get_input();
struct File list();
void upload();
void download();
void delete();
void quit();
void print_options();
void input_option();
void print_welcome();

void test_list();
void test();

// Functions

void get_input(){
	printf("> ");
	input = malloc(20*sizeof(char));
	scanf("%s", input);	
}

struct File list(){
	struct File *root;
	root = (struct File *)malloc(sizeof(struct File));
	root->next = NULL;

	struct File *current;
	current = root;

	DIR *dir;
	struct dirent *ent;
	dir = opendir(path);

	if(dir){
		printf("Here are the list of files:\n");
		printf("----------------------------------------------");	
		while((ent = readdir(dir)) != NULL){
			if(ent->d_type != 4){	
				if(root->next != NULL){
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
				strcat(file_path, filename);
				stat(file_path, &file_stats);

				current->filename = filename;
				current->size = (long long)file_stats.st_size;

				printf("\n%s (%lld bytes)", current->filename, current->size);

				free(filename);
				free(file_path);
			}
		}
		printf("\n----------------------------------------------\n\n");	
	}

	else {
		printf("There are no files available for download.\n");		
	}

	free(ent);
	free(dir);
	free(current);
	
	return *root;
}

// void print_list(struct File *root){
// 	struct File *current;
// 	*current = root;

// 	printf("Here are the list of files:\n");
// 	printf("----------------------------------------------");	
// 	while (current->next != NULL){
// 		printf("\n%s (%lld bytes)", current->filename, current->size);
// 		current = current->next;
// 	}
// 	printf("\n----------------------------------------------\n\n");	

// 	free(current);
// 	// free(root);
// }

void upload(){
	printf("What file would you like to upload?\n");
	get_input();

	free(input);
}

void download(){
	printf("What file would you like to download?\n");
	get_input();

	free(input);
}

void delete(){
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

void print_welcome(){
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	
	printf("Welcome to BitDrive! \n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");		
}

// Test Functions

void test(){
	test_list();
};

void test_list(){
	// print_list(list());
	list();
}

int main(){
	// print_welcome();

	// while(loop == true){
	// 	print_options();
	// 	input_option();
	// }

	test();

	return 0;
}

