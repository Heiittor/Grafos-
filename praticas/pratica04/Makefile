CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.
TARGET = main
# Como o grafo_lista.c está sendo incluído dentro do main.c, não colocamos ele aqui!
SRCS = main.c conectividade.c planaridade.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(TARGET).exe

.PHONY: all clean