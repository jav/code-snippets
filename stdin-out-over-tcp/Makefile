CC = gcc
CFLAGS = -Wall -ansi

#LDFLAGS = -L/usr/local/lib
#LDLIBS = -llib


all: sender receiver

sender.o: sender.c
	$(CC) -c $(CFLAGS) -o sender.o sender.c

gopt.o: gopt.c
	$(CC) -c $(CFLAGS) -o gopt.o gopt.c

sender: sender.o gopt.o
	$(CC) $(LDFLAGS) sender.o gopt.o -o sender $(LDLIBS)

receiver:

clean:
	rm *.o sender receiver	
