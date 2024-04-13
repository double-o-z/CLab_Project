CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: assembler

assembler: Main.c Assembler.c FileParser.c MacroProcessor.c FirstPass.c SecondPass.c CreateOutput.c Helper.c FirstPassHelper.c Opcodes.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f assembler

