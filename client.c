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
void free_list(struct File *head);

struct File* list();
void print_list(struct File *root);
char *list_to_string(struct File *root);

void upload();
void download();
void delete();
void quit();

void print_options();
void input_option();
void print_welcome();

void test_list();
void test_list_to_string();
void test();

// Functions

void get_input(){
	printf("> ");
	input = malloc(20*sizeof(char));
	scanf("%s", input);	
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

struct File* list(){
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

void print_list(struct File* root){
	struct File *current;
	current = root;

	printf("Here are the list of files:\n");
	printf("----------------------------------------------");	
	while (current != NULL){
		printf("\n%s (%lld bytes)", current->filename, current->size);
		current = current->next;
	}
	printf("\n----------------------------------------------\n\n");

	free_list(root);
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
		char buffer[100];
		sprintf(buffer, "%llu", current->size);
		strcat(list_string, current->filename);
		strcat(list_string, "\n");
		strcat(list_string, buffer);
		strcat(list_string, "\n");
		current = current->next;
	}

	free_list(root);
	return list_string;
}

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
	print_list(list());
	test_list();
	test_list_to_string();
};

void test_list(){
	printf("\n=========================\n");	
	printf("TEST: list()\n");
	printf("=========================\n");	

	struct File *root = list();
	struct File *current;
	current = root;

	while (current != NULL){
		if(strcmp(current->filename, "readme.txt") == 0){
				if(current->size == 44){
					printf("Struct for %s: PASS\n", current->filename);
				}
				else {
					printf("Struct for %s: FAIL (wrong size)\n", current->filename);
				}
		}

		else if(strcmp(current->filename, "lorem.txt") == 0){
				if(current->size == 244){
					printf("Struct for %s: PASS\n", current->filename);
				}
				else {
					printf("Struct for %s: FAIL (wrong size)\n", current->filename);
				}
		}

		else{
			printf("Unknown struct: %s (%llu bytes)\n", current->filename, current->size);
		}

		current = current->next;
	}

	free_list(root);
}

void test_list_to_string(){
	printf("\n=========================\n");	
	printf("TEST: list_to_string()\n");
	printf("=========================\n");	
	char *list_string = list_to_string(list());

	if(strcmp(list_string, "readme.txt\n44\nlorem.txt\n244\n") == 0){
		printf("List to String: PASS\n");
	}

	else {
		printf("List to String: FAIL\n");
	}
	
	free(list_string);
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

