CC = gcc
CFLAGS = -Wall -ansi -pedantic

all: assembler

assembler: Main.c Assembler.c FileParser.c MacroProcessor.c FirstPass.c SecondPass.c \
CreateOutput.c Helper.c FirstPassHelper.c Opcodes.c SecondPassHelper.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f assembler *.am *.ent *.ext *.ob

