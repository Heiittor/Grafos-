CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = main
ifeq ($(OS),Windows_NT)
TARGET = main.exe
SHELL = cmd.exe
endif
SRCS = main.c dijkstra.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.c dijkstra.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: $(TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TARGET)
else
	./$(TARGET)
endif

clean:
ifeq ($(OS),Windows_NT)
	powershell.exe -NoProfile -Command "foreach ($$arquivo in @('main.o', 'dijkstra.o', 'main', 'main.exe', 'a.out')) { if (Test-Path -LiteralPath $$arquivo) { Remove-Item -LiteralPath $$arquivo -Force -ErrorAction Stop } }"
else
	rm -f $(OBJS) main main.exe a.out
endif

.PHONY: all test clean
