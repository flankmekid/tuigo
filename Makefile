CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
TARGET = tuigo

ifdef MSYSTEM
	CFLAGS += -D_XOPEN_SOURCE_EXTENDED=1
	LIBS    = -lncursesw -lm
else ifeq ($(OS),Windows_NT)
	LIBS    = -lpdcurses -lm
	TARGET  = tuigo.exe
else
	CFLAGS += -D_XOPEN_SOURCE_EXTENDED=1
	LIBS    = -lncursesw -lm
endif

SRCS = src/main.c src/board.c src/rules.c src/score.c \
       src/render.c src/input.c src/menu.c src/game.c src/ai.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o tuigo tuigo.exe

.PHONY: all clean
