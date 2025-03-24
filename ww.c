#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define S_ISDIR
#define S_ISREG

/**
 * Outputs a new line as needed.
 **/
int addNewLine(int output_fd){
	char* newLine = malloc(sizeof(char));
	newLine[0] = '\n';
	write(output_fd, &newLine[0], 1);
	free(newLine);
	return 0;
}

int addSpace(int output_fd){
	char* newLine = malloc(sizeof(char));
	newLine[0] = ' ';
	write(output_fd, &newLine[0], 1);
	free(newLine);
	return 0;
}

int wrap(unsigned width, int input_fd, int output_fd){
	
	char *buffer = malloc(sizeof(char) * width);
	char *stash = malloc(sizeof(char) * (width + 1)); int prev_space2 = 0;
	int bytes = 0; int bytes2; int start = 0; int byte_size = width; int end = width - 1; int exceed_width = 0; int same_line = 0; int prev_space = 0;
	while ((bytes = read(input_fd, &buffer[start], byte_size)) > 0){
		if (byte_size){
			//have this look at exceeded width
			int check3 = 0;
			for (int j = 0; j < width; j++){
				if (isspace(buffer[j])){
					check3 = 1;
					break;
				}
			}
			if (check3 == 0){
				exceed_width = 1;
				start = 0;
				end = width - 1;
				bytes2 = write(output_fd, buffer, width);
				for (int i = 0; i < width; i++){
					buffer[i] = ' ';
				}
				continue;
			}
			for (int i = 0; i < width; i++){
				if (buffer[i] == '\t'){
					buffer[i] = ' ';
				}
			}
			int change = -1; //this marks the first nonspace after two \n
			//int begin = -1; //this marks the first newline 
			for (int i = 0; i < width; i++){
				if (buffer[i] == '\n'){
					//begin = i;
					int delete = 1;
					/*
					if (i == width - 1){
						delete = 0;
					} */
					for (int j = i + 1; j < width; j++){
						//if we find a newline character after we already found one, make it a space
						if (buffer[j] == '\n' && delete == 0){
							buffer[j] = ' ';
						}
						//if we find a second newline, mark it with change and make delete false
						if (buffer[j] == '\n' && delete == 1){
							//change = j
							delete = 0;
						}
						//stop once we hit a nonspace
						if (!(isspace(buffer[j]))){
							if (delete == 0){
								change = j;
							}
							break;
						}
					}
					//if we didn't find a second newline
					if (delete == 1){
						buffer[i] = ' ';
						//begin = -1;
					}
					//if we did, we should halt this loop, get rid of the space, and call read again
					else { break; }
				}
				
			}
			if (change != -1){ //we should write after this
				//have this loop delete space between the first newline and the last nonspace before it
				if (prev_space == 0){
					if (prev_space2 == 0){
						addNewLine(output_fd);
					}
				}
				prev_space = 1; prev_space2 = 0;
				int diff = change;
				for (int i = change - 1; i >= 0; i--){
					if (isspace(buffer[i])){
						diff--;
					}
					else { break; }	
				}
				int k = 0;
				for (int i = change; i < width; i++){
					stash[k] = buffer[i];
					k++;
				}
				stash[k] = '\0';
				bytes2 = write(output_fd, buffer, diff);
				addNewLine(output_fd); 
				addNewLine(output_fd);
				for (int i = 0; stash[i] != '\0'; i++){
					buffer[i] = stash[i];
				} 
				for (int i = k; i < width; i++){
					buffer[i] = ' ';
				}
				start = k;
				byte_size = width - start;
				end = width - 1;
				
			//	continue;
			}
			//check if buffer is all space
			int check = -1;
			for (int i = 0; i < width; i++){
				if (!isspace(buffer[i])){
					check = i;
				}
			}
			if (check == -1){
				continue;
			}
			check = 0;
			//this part used to get rid of spaces at the start of the buffer
			for (int i = 0; i < width; i++){
				if (!(isspace(buffer[i]))){
					check = i;
					break;
				}
			}
			if (check != 0){
				int k = 0;
				for (int i = check; i < width; i++){
					stash[k] = buffer[i];
					k++;
				}
				stash[k] = '\0';
				for (int i = 0; stash[i] != '\0'; i++){
					buffer[i] = stash[i];
				} 
				for (int i = k; i < width; i++){
					buffer[i] = ' ';
				}
				start = k;
				byte_size = width - start;
				end = width - 1;
				prev_space = 1;
				continue;
			}
			//this is for space between words
			int keep_space = 0;
			if (isspace(buffer[end]) && !(isspace(buffer[end - 1]))){
				keep_space = 1;
			}
			int mark = -1; int mark2 = 0; int x = 0; int stop = width - 1;//mark used for start on nonspace bytes, mark2 is byte after space
			for (int i = 0; i < width; i++){
				if (isspace(buffer[i])){
					if (i == 0){
						for (int o = 0; o < width - 1; o++){
							buffer[o] = buffer[o + 1];
						}
						buffer[width - 1] = ' ';
						continue;
					}
					mark2 = i + 1;
					for (int j = i + 1; j < width; j++){
						if (!(isspace(buffer[j]))){
							mark = j;
							break;
						}
					}
					if (mark != -1 && mark != i + 1){
						stop = mark - i - 1;
						while (mark < width){
							stash[x] = buffer[mark];
							mark++; x++;
						}
						stash[x] = '\0'; x = 0;
						for (int y = mark2; stash[x] != '\0'; y++){
							buffer[y] = stash[x];
							x++;
						}
						
						for (int a = width - 1; stop != 0; a--){
							buffer[a] = ' ';
							stop--;
						} 
						
					}
					//else { break; }
				}
				x = 0; mark = -1;
			}
			int new_byte_size = 0; int new_start = width;
			for (int i = width - 1; i >= 0; i--){
				if (isspace(buffer[i])){
					new_byte_size++;
					new_start--;
				}
				else { break; }
			}
			if (keep_space == 1){
				new_byte_size--;
				new_start++;
			}
			if (new_byte_size != 0){
				byte_size = new_byte_size; 
				start = new_start;
				end = width - 1;
				//continue;
			} 	
			if (new_byte_size != 0 || change != -1){
				//prev_space = 1;
				continue;
			}	
			//this checks if a space starts off 
			if (isspace(buffer[start])){
				//if this happens, we know we can put the first few bytes on the previous line
				if (start != 0){
					if (prev_space == 0){
						addSpace(output_fd);
						buffer[start] = '\n';
						//prev_space = 1;
					}
					else {
						prev_space = 0;
					}
					//buffer[start] = '\n';
					bytes2 = write(output_fd, buffer, (start + 1));
					int k = 0;
					for (int j = start + 1; j < width; j++){
						stash[k] = buffer[j];
						k++;
					}
					stash[k] = '\0';
					for (int i = 0; stash[i] != '\0'; i++){
						buffer[i] = stash[i];
					}
					for (int i = k; i < width; i++){
						buffer[i] = ' ';
					}                     
					start = k;
					byte_size = width - start;
					end = width - 1;
					prev_space = 1;
					
				}
				//if the first byte in buffer is space, we change it to newline and shift over the whole buffer and read one byte next
				else {
					if (exceed_width == 1){
						addNewLine(output_fd);
						exceed_width = 0;
					} 
					for (int i = 1; i < width; i++){
						buffer[i - 1] = buffer[i];
					}
					start = width - 1;
					byte_size = 1;
					end = width - 1;
				}
			}
			else if (!(isspace(buffer[end]))){
				
				if (exceed_width == 1){
					int check4 = -1;
					for (int i = 0; i < width; i++){
						if (isspace(buffer[i])){
							check4 = i;
						}
					}
					bytes2 = write(output_fd, buffer, check4);
					addNewLine(output_fd);
					int k = 0;
					for (int i = check4 + 1; i < width; i++){
						stash[k] = buffer[i];
						k++;
					}
					stash[k] = '\0';
					for (int i = 0; stash[i] != '\0'; i++){
						buffer[i] = stash[i];
					}
					for (int i = k; i < width; i++){
						buffer[i] = ' ';
					}
					start = k;
					exceed_width = 0;
					end = width - 1;
					byte_size  = width - start;
					continue;
				}
				//this adds a newline if start isn't 0 and isn't a space, for the previous line
				if (!isspace(buffer[start]) && start != 0){
					if (prev_space == 0){
						addNewLine(output_fd);
					}
					prev_space = 0;
					start = 0;
				}	
				//this tells us where we can cut off the buffer to fit bytes on the line
				for (int i = start; i < width; i++){
					if (isspace(buffer[i])){
						end = i;
					}
				}
				
				int k = 0;
				//buffer[end] = '\n';
				end++; //update end so we print out space, may need to change this for \n
				//stash bytes after the last space
				for (int j = end; j < width; j++){
					stash[k] = buffer[j];
					k++;
				}
				end--; //new change here
				stash[k] = '\0'; //stash[0] = '\n';
				//write out bytes we can fit
				bytes2 = write(output_fd, buffer, end);
				//overwrite the stashed stuff in the beginning of buffer
				for (int i = 0; stash[i] != '\0'; i++){
					buffer[i] = stash[i];
				}
				//this makes the rest of the buffer contain whitespace - maybe not needed
				for (int i = k; i < width; i++){
					buffer[i] = ' ';
				}
				start = k; // update to make sure we start reading here
				byte_size = width - start;	
				end = width - 1;
			}
			else if (isspace(buffer[end])){ //if we do end on a space
				//this here adds a newline character if we cut the previous line
				if (!isspace(buffer[start]) && start != 0){
					for (int i = 0; i < width; i++){
						stash[i] = buffer[i];
					}	
					if (prev_space == 0){
						addNewLine(output_fd);
						//prev_space = 1;
					}
					prev_space = 0;
					for (int i = 0; i < width; i++){
						buffer[i] = stash[i];
					}	
				}
				//if (prev_space == 0){
				buffer[end] = '\n';
				//}
				prev_space = 0;
				prev_space2 = 1;
				byte_size = width;
				bytes2 = write(output_fd, buffer, byte_size);
				start = 0;
				end = width - 1;
			}
		}
		for (int i = start; i < width; i++){
			buffer[i] = ' ';
		}
	}
	for (int i = 0; i < width; i++){
		if (buffer[i] == '\t'){
			buffer[i] = ' ';
		}
	}
	int check2 = -1;
	for (int i = 0; i < width; i++){
		if (!isspace(buffer[i])){
			check2 = i;
		}
	}
	if (check2 == -1){
		free(stash); free(buffer);
		return bytes2;
	}
	if (prev_space == 0){
		addNewLine(output_fd);
	}
	byte_size = width;
	for (int i = width - 1; i >= 0; i--){
		if (isspace(buffer[i])){
			byte_size--;
		}
		else { break; }
	}
	for (int i = 0; i < byte_size; i++){
		if (buffer[i] == '\n'){
			buffer[i] = ' ';
		}
	}
	if (byte_size < width && byte_size != 0){
		buffer[byte_size] = '\n';
		byte_size++;
	}
	if (byte_size != 0){
		bytes2 = write(output_fd, buffer, byte_size);
	}
	free(stash); free(buffer);
	return exceed_width;
}

int checkDir(const char *path) {
	struct stat stat_path;
	int check = stat(path, &stat_path);

	if (check < 0) { 
		printf("Error");
		return check;
	}
	
	return S_ISDIR(stat_path.st_mode);
}

int main(int argc, char **argv)
{
    int input_fd = 0;
    int output_fd = 1;
    unsigned width;
    DIR *dir_input_fd;
	int dir = 0;
    int error = 0;
    int error_dir = 0;

    // Ensure appropriate number of arguments
    if (argc < 2) {
        printf("Not enough arguments.\n");
        return EXIT_FAILURE;
    }

    // If argc == 3, read file or directory
    else if (argc == 3) {
		printf("Read from a file or directory.\n\n");
		//struct stat path_stat;
		//printf("%s\n", argv[2]);
		//stat(argv[2], &path_stat);
		dir = checkDir(argv[2]);
		//printf("Dir: %d\n", dir);
		dir_input_fd = opendir(argv[2]);

		if (dir == 16877) {
			printf("Directory!\n");

		} else if (dir == 33188) {
			input_fd = open(argv[2], O_RDONLY);
			output_fd = 1;
			printf("File!\n");
		}

        // Return error if file can't be opened
        if (input_fd == -1) {
            printf("Not a file\n");
            perror(argv[2]);
            return EXIT_FAILURE;
        }
    }

    // If argc == 2, read from stdin
    else if (argc == 2) {
        input_fd = 0;
        printf("Read from STDIN\n\n");
    }

	else if (argc > 3) {
		printf("Invalid # of arguments.\n");
		return EXIT_FAILURE;
	}

    // Ensure width value is a valid positive integer
    if (atoi(argv[1]) <= 0) {
        printf("Please input a valid positive integer value for width.\n");
        return EXIT_FAILURE;
    }

    width = atoi(argv[1]);

    if (argc == 3 && dir_input_fd) {
        struct dirent *dirp;
        size_t dir_len = strlen(argv[2]);
        char *path_to_dir = malloc(dir_len+1);
        if (!path_to_dir) return 1;
        strcpy(path_to_dir, argv[2]);

        size_t prefix_length = strlen("wrap.");

        while ((dirp = readdir(dir_input_fd)) != NULL) {
            size_t fname_len = strlen(dirp->d_name);
            if (dirp->d_name[0] == '.') {   // skip if file begins with .
                continue;
            }
            if (fname_len >= 4) {   // skip if file begins with wrap
                if (dirp->d_name[0] == 'w' && dirp->d_name[1] == 'r' && dirp->d_name[2] == 'a' && dirp->d_name[3] == 'p') {
                    continue;
                }
            }
			
            char *name_input;
            char *name_output;

            int input_name_size = dir_len + fname_len;
            name_input = malloc(input_name_size+1);
            if (!name_input) return 1;
            memcpy(name_input, path_to_dir, dir_len);
            memcpy(name_input + dir_len, &(dirp->d_name), fname_len + 1);

			if (checkDir(name_input) == 16877) {
				free(name_input);
				continue;
			} else if (checkDir(name_input) == 33188) {
					// Create output file name from directory path and input
					int output_name_size = dir_len + fname_len + prefix_length;
					name_output = malloc(output_name_size+1);
					if (!name_output) return 1;
					memcpy(name_output, path_to_dir, dir_len);
					memcpy(name_output + dir_len, "wrap.", prefix_length + 1);
					memcpy(name_output + dir_len + prefix_length, &(dirp->d_name), fname_len + 1);

					input_fd = open(name_input, O_RDONLY);
					output_fd = open(name_output, O_WRONLY | O_TRUNC | O_CREAT, 0600);
					error_dir = wrap(width, input_fd, output_fd); //file descriptor for the new file it writes to

					free(name_output);
			}
			close(input_fd);
			close(output_fd);
            free(name_input);
        }
        
        closedir(dir_input_fd);
        free(path_to_dir);
    }
    else {
        error = wrap(width, input_fd, 1);
        close(input_fd);
    }

    if (error == 1 || error_dir == 1) { //a change here for directories
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}