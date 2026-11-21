CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = main
ifeq ($(OS),Windows_NT)
TARGET = main.exe
SHELL = cmd.exe
endif
SRCS = main.c hamiltoniano.c tps.c
OBJS = $(SRCS:.c=.o)
HEADERS = hamiltoniano.h tps.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: $(TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TARGET)
else
	./$(TARGET)
endif

clean:
ifeq ($(OS),Windows_NT)
	powershell.exe -NoProfile -Command "foreach ($$arquivo in @('main.o', 'hamiltoniano.o', 'tps.o', 'main', 'main.exe', 'a.out')) { if (Test-Path -LiteralPath $$arquivo) { Remove-Item -LiteralPath $$arquivo -Force -ErrorAction Stop } }"
else
	rm -f $(OBJS) main main.exe a.out
endif

.PHONY: all test clean
