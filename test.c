char *list_to_string(struct File *root){
	struct File *current;
	current = root;
	long long size = 0;
	int file_counter = 0;

	// get size for malloc
	while (current->next != NULL){
		size += sizeof(*current);
		file_counter++;
		printf("%s\n", current->filename);
		current = current->next;
	}

	// char *list = malloc(size + (2*file_counter) + file_counter);
	// strcpy(list, "\0");
	// current = &root;	
	// while (current->next != NULL){
	// 	// char buffer[100];
	// 	// sprintf(buffer, "%llu", current->size);
	// 	printf("%s\n", current->filename);
	// 	// strcat(list, current->filename);
	// 	// strcat(list, "\n");
	// 	// strcat(list, buffer);
	// 	// strcat(list, "\n");
	// }

	// printf("%s\n", list);
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