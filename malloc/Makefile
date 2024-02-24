CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wvla
LDFLAGS = -shared
VPATH = src

TARGET_LIB = libmalloc.so
OBJS = malloc.o utils.o

all: library

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -pedantic -fvisibility=hidden -fPIC
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS += -g
debug: clean $(TARGET_LIB)

check: main

main: CFLAGS += -g
#main: CPPFLAGS += -DDEBUG
main: library
	$(CC) $(CFLAGS)  src/main.c src/utils.c -L. -lmalloc -o main
	LD_LIBRARY_PATH=. ./main

clean:
	$(RM) $(TARGET_LIB) $(OBJS) main

.PHONY: all $(TARGET_LIB) clean
