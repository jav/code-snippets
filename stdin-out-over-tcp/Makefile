CC = gcc
CFLAGS = -Wall -ansi

#LDFLAGS = -L/usr/local/lib
#LDLIBS = -llib


all: sender receiver


gopt.o: gopt.c
	$(CC) -c $(CFLAGS) -o gopt.o gopt.c

sender.o: sender.c
	$(CC) -c $(CFLAGS) -o sender.o sender.c

sender: sender.o gopt.o
	$(CC) $(LDFLAGS) sender.o gopt.o -o sender $(LDLIBS)

reciever.o: reciever.c
	$(CC) -c $(CFLAGS) -o reciever.o reciever.c

receiver: reciever.o gopt.o
	$(CC) $(LDFLAGS) reciever.o gopt.o -o receiver $(LDLIBS)

clean:
	rm *.o sender receiver	
