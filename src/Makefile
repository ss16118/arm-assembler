CC      = gcc
CFLAGS  = -p -Wall -std=c99 -pedantic

.SUFFIXES: .c .o

.PHONY: clean

all: emulate assemble


emulate: emulate.o emulator.o utility.o ARM.o emulatorIO.o
	$(CC) -o emulate emulate.o emulator.o utility.o ARM.o emulatorIO.o -lm


emulator.o: emulator.c utility.h emulator.h ARM.h structs_and_constants.h
	$(CC) $(CFLAGS) emulator.c -c -o emulator.o	


utility.o: utility.c utility.h structs_and_constants.h
	$(CC) $(CFLAGS) utility.c -c -o utility.o


ARM.o: ARM.c ARM.h utility.h structs_and_constants.h
	$(CC) $(CFLAGS) ARM.c -c -o ARM.o


emulatorIO.o: emulatorIO.c emulatorIO.h ARM.h utility.h structs_and_constants.h
	$(CC) $(CFLAGS) emulatorIO.c -c -o emulatorIO.o



assemble: assemble.o assembler.o utility.o assemblerIO.o symbolTable.o structs_and_constants.h
	$(CC) -o assemble assemble.o assembler.o utility.o assemblerIO.o symbolTable.o -lm


assembler.o: assembler.c symbolTable.h assembler.h utility.h structs_and_constants.h
	$(CC) $(CFLAGS) assembler.c -c -o assembler.o


assemblerIO.o : assemblerIO.c assembler.h assemblerIO.h structs_and_constants.h
	$(CC) $(CFLAGS) assemblerIO.c -c -o assemblerIO.o


symbolTable.o: symbolTable.c symbolTable.h utility.h assembler.h structs_and_constants.h
	$(CC) $(CFLAGS) symbolTable.c -c -o symbolTable.o


clean:
	rm -f ARM.o emulate.o emulator.o emulatorIO.o utility.o
	rm -f assembler.o assemble.o symbolTable.o assemblerIO.o
