#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>


/**
 * This will be the method we call individually on each file in a directory. 
 * Takes a file pointer and wraps the file. 
 */
int wrapFile(FILE *input_file, char *input_file_name, int line_length) {

    //assemble name for output file
    char *start = (char*)strrchr(input_file_name, '/'); //get last occurrence of / in file path
    char *end = (char *)memchr(input_file_name, '.', strlen(input_file_name)); //find . in file name
    *end = '\0';
    char *output_file_name = malloc(sizeof(char) * (24 + strlen(input_file_name)));
    output_file_name[0] = '\0';
    strcat(output_file_name, "output_files/");
    strcat(output_file_name, start + 1);
    strcat(output_file_name, "_output.txt");

    FILE *output_file = fopen(output_file_name, "w"); //output file, should create/overwrite file
    free(output_file_name);

    //initialize buffer to store chars. Is length of inputted length plus one for \0
    char *buffer = malloc(sizeof(char) * (line_length + 1)); //account for null char
    buffer[0] = '\0';

    //initialize counters
    int index = 0; //current index in buffer
    char byte; //current char
    int space = 0; //boolean if coming off of space char
    int new_line = 0; //counter for consecutive newlines. 
    int cutoff = -1; //counter for last space char

    //every iteration gets next char until EOF
    while ((byte = getc(input_file)) != EOF) {

        if (isspace(byte)){ //check if space

            if (byte == '\n'){ //check if newline
                new_line++;
                if (new_line >= 2){
                    continue;
                }
            }
            if (space == 1) { //if any other space, just skip
                continue;
            }

            space = 1; //mark a streak of spaces
            buffer[index] = ' '; //mark next char as space
            cutoff = index; //mark this as cutoff since its the newest space char 
            index++;
            buffer[index] = '\0';

        } else { //else, the char is text

            if (new_line >= 2 && index != 0){ //write to file if paragraph separation
                new_line = 0; space = 0; cutoff = -1;
                fprintf(output_file, "%s", buffer);
                fprintf(output_file, "\n\n");
                buffer[0] = byte;
                index = 1;
                buffer[index] = '\0';
                continue;
            }

            if (index >= line_length){ //if we reach the max length of line

                if (space == 1){ //see if we're coming off a space character

                    fprintf(output_file, "%s", buffer); //just print the entire line
                    fprintf(output_file, "\n");
                    buffer[0] = byte;
                    index = 1;
                    buffer[index] = '\0';
                    space = 0;

                } else { //else see what the last space char was

                    if (cutoff == -1){ //if we have no cutoff marker, can't make it work. Exit

                        return 1;

                    } else { //find last cutoff marker and print up to that

                        buffer[cutoff] = '\0'; //mark cutoff as end of string
                        fprintf(output_file, "%s", buffer);
                        fprintf(output_file, "\n");

                        //shift leftover chars to front of buffer
                        index = 0; cutoff++;
                        while (cutoff < line_length){
                            buffer[index] = buffer[cutoff];
                            index++; cutoff++;
                        }

                        //add the current byte
                        buffer[index] = byte;
                        index++;
                        buffer[index] = '\0';
                    }
                }
                cutoff = -1; //since there's no space char, reset cutoff marker
                new_line = 0;
                continue;
            }

            buffer[index] = byte;
            index++;
            buffer[index] = '\0';
            space = 0;
            new_line = 0;

        }
        
    }

    //print whatever is left
    fprintf(output_file, "%s", buffer);

    //close the file pointers
    fclose(output_file);
    fclose(input_file);
    free(buffer); //free memory

    return 0;
}


/**
 * Main method that is called when the program starts. The main purpose is to handle command line arguments.
 * Check to make sure no invalid arguments are input, and check to see if the input is a directory or file.
 */
int main(int argc, char **argv){

    //check if 2 arguments, exit otherwise
    if (argc != 3){
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    if (atoi(argv[2]) <= 0) { //if given number is 0 or less, exit
        printf("Invalid required character limit entered.\n");
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(argv[1]); //first lets see if its a directory.
    if (dir){

        struct dirent *entry; //struct to hold file entries in directory
        while ((entry = readdir(dir)) != NULL){ //go through every entry
            //need to combine working dir name and the file entry name
            char *file_name = malloc(sizeof(char) * (strlen(argv[1]) + strlen(entry->d_name) + 2));
            file_name[0] = '\0';
            strcat(file_name, argv[1]);
            strcat(file_name, "/");
            strcat(file_name, entry->d_name);

            FILE *file = fopen(file_name, "r"); //open file entry
            if (file == NULL) {
                free(file_name);
                continue;
            }
            if (wrapFile(file, file_name, atoi(argv[2])) == 1){
                printf("A file in the directory had a word larger than the entered character limit.\n");
                free(file_name);
                return EXIT_FAILURE;
            }
            free(file_name);
        }

        closedir(dir);
        return EXIT_SUCCESS;

    }

    int line_length = atoi(argv[2]); //length of lines in output file
    FILE *input_file = fopen(argv[1], "r"); //FILE pointer to input file

    if (input_file == NULL){
        printf("Could not find file or directory.\n");
        return EXIT_FAILURE;
    }

    if (wrapFile(input_file, argv[1], atoi(argv[2])) == 1){ //exit failure if returns 1 (fail)
        printf("A word in the file is too long for the inputted character limit.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}