CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: assembler

assembler: Main.c FileParser.c MacroProcessor.c Assembler.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f assembler

