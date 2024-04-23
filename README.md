# CLab_Project
Final project for Software Lab with C course in Open University of Israel

# Assembler
This project simulates an assembler for a simulated predefined CPU.
It is called from the command-line, like so:
assembler [-d] <input_file1> [<input_file2> ...]
Whereas "-d" is an optional debug flag, and input files are to be processed by the assembler.

The assembler has several logical parts:
1. Parser.
2. Preprocessor (macro processing).
3. First Pass. (Handle all data in the code).
4. Second Pass. (Handle all instructions in the code).
5. Create Output. (Prepares all the necessary output file, if possible).

During its run, the assembler prints errors with line number where is should be fixed in the input file.

The assebmler has a makefile, and here is how to call make for it:
make clean && make

And here is how to run the resulting assemlber object with input (file1.as, file2.as):
./assembler -d file1 file2

This will invoke the assembler, and will handle both file1 and file2, and create output for each.

The assembler simulates 16 different commands, and 4 different operand types.
It uses a memory system with up to 4096 words, each consisting of 14 bits.


By Or Ohev-Zion, 304839376
18.04.24
