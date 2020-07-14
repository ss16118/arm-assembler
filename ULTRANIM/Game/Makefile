CC      = gcc
CFLAGS  = -p -Wall -std=c99 -pedantic

.SUFFIXES: .c .o

.PHONY: clean

game: game.o io.o playGame.o setup.o utility.o nimatron.o
	$(CC) -o game game.o io.o playGame.o setup.o utility.o nimatron.o

game.o: game.c game.h io.h playGame.h setup.h
	$(CC) $(CFLAGS) game.c -c -o game.o

setup.o: setup.c setup.h io.h utility.h
	$(CC) $(CFLAGS) setup.c -c -o setup.o

playGame.o: playGame.c playGame.h io.h utility.h
	$(CC) $(CFLAGS) playGame.c -c -o playGame.o

io.o: io.c io.h
	$(CC) $(CFLAGS) io.c -c -o io.o

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) utility.c -c -o utility.o

nimatron.o: nimatron.c nimatron.h
	$(CC) $(CFLAGS) nimatron.c -c -o nimatron.o

clean:
	rm -f *.o 
