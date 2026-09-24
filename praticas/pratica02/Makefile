CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I. -I../pratica01
TARGET = main
SRCS = main.c busca_largura.c busca_profundidade.c ../pratica01/grafo_lista.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(TARGET).exe ../pratica01/*.o

.PHONY: all clean