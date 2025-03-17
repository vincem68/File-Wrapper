#include <stdio.h>
#include <stdlib.h>

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

        } else { //else, the char is text

            if (new_line >= 2 && index != 0){ //write to file if paragraph separation
                new_line = 0; space = 0; cutoff = -1;
                fprintf(output_file, "%s", buffer);
                fprintf(output_file, "\n\n");
                index = 0; buffer[index] = '\0';
                continue;
            }

            if (space == 1) { //if we only got a newline
                new_line = 0; space = 0;
                cutoff = index;
                buffer[index] = ' ';
                index++;
            }

            buffer[index] = byte;
            index++;
            buffer[index] = '\0';

            if (index == line_length) { //if we reach the max amount of chars

                byte = getc(input_file); //get next char

                if (isspace(byte) || byte == EOF){ //see if space or EOF

                    if (byte == '\n'){ //if newline, increment counter and loop to find next
                        new_line++;
                    }

                    fprintf(output_file, "%s", buffer); //output the entire buffer to file

                    index = 0; //reset index and buffer
                    buffer[index] = '\0';

                } else { //if char is text

                    if (cutoff == -1) { //if no space in buffer, not valid. exit
                        return EXIT_FAILURE;
                    }

                    buffer[cutoff] = '\0'; //print chars in buffer up to last space in buffer
                    fprintf(output_file, "%s", buffer);

                    //shift leftover chars to front of buffer
                    index = 0; cutoff++;
                    while (cutoff < line_length){
                        buffer[index] = buffer[cutoff];
                        index++; cutoff++;
                    }
                    buffer[index] = '\0';
                }

                cutoff = -1; //reset cutoff counter
            }

        }
        
    }

    //print whatever is left
    fprintf(output_file, "%s", buffer);

    fclose(output_file);
    fclose(input_file);
    free(buffer);

    return EXIT_SUCCESS;
}