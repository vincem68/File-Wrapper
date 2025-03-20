#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(){

    int line_length = 50; //length of lines in output file
    FILE *input_file = fopen("notes.txt", "r"); //FILE pointer to input file
    FILE *output_file = fopen("output.txt", "w"); //output file, should create/overwrite file

    if (input_file == NULL){ //exit if input file does not exist
        printf("No file exists.\n");
        return EXIT_FAILURE;
    }

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

                        return EXIT_FAILURE;

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

    return EXIT_SUCCESS;
}