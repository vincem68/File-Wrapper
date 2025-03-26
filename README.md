*******File Wrapper Project Revised********

This is a revision of a systems programming project I did about 4 years ago back in the Fall of 2020.

The program is a File Wrapper. Given the arguments of a name of a text file and a number of 1 or greater, the program will reformat the file to fit the text into lines 
containing no more characters than the number argument given. Consecutive spaces will be eliminated (if you come across something like "example \t \t example", it will become "example example" in the output file). This will include any space characters such as tabs, newlines, etc. The exception is that paragrapghs will be separated by two newlines.
If the input file has two or more newline characters within consective whitespace, the output file will keep those paragrapghs. One newline will just be turned to a regular
' ' character. 

The program will take 3 arguments from the command line in the format: (name of executable) (name of text file) (number limit of characters to fit on a single line).
File given must be a text file and will output the formatted file into a separate subdirectory. Number given must be positive and greater than 0.

Words cannot be cut off. If the file has a word that has more characters than the given character limit, it will exit immediately.

You can also enter the name of a directory. In doing this, the program will look at every text file within the 
directory and wrap them and put them in a directory named output_files. 
**************

Note: This is a project I've revisited to make better. I worked with a partner on the original version which is ww.c, but was not happy about it since it was not perfect
and was very messy code containing about 560 lines of code. The revised version, wrapper.c, aimed to be a lot more neat, less redundant, and easier to understand while being
bug free and more efficient in runtime and memory management. Take a look at the two files to compare with each other, and how much better at programming I've become since.

