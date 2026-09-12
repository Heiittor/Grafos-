CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

OBJS = main.o dag.o

main: $(OBJS)
	$(CC) $(CFLAGS) -o main $(OBJS)

main.o: main.c dag.h
	$(CC) $(CFLAGS) -c main.c

dag.o: dag.c dag.h
	$(CC) $(CFLAGS) -c dag.c

clean:
	rm -f *.o main main.exe