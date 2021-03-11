CC=gcc
CFLAGS=-W -Wall
EXEC_NAME=mishell
HEADER_FOLDER=header

all: main.o pars.o primitive.o
	$(CC) -o $(EXEC_NAME) main.o pars.o primitive.o

main.o: main.c $(HEADER_FOLDER)/primitive.h $(HEADER_FOLDER)/colors.h $(HEADER_FOLDER)/pars.h
		$(CC) -o main.o -c main.c $(CFLAGS)

pars.o: pars.c $(HEADER_FOLDER)/pars.h
				$(CC) -o pars.o -c pars.c $(CFLAGS)

primitive.o: primitive.c $(HEADER_FOLDER)/primitive.h
				$(CC) -o primitive.o -c primitive.c $(CFLAGS)




clean:
	rm -rf *.o
	rm $(EXEC_NAME)
