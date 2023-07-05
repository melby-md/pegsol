CC = cc -std=c99
SDL_CONFIG = sdl2-config
SDL_FLAGS = --cflags --libs
WARNINGS = -Wall -Wextra -Wvla -Wno-unused-parameter -Wno-unused-function
BIN = pegsol$(EXE)

all: $(BIN)

$(BIN): main.c optparse.h
	$(CC) $(WARNINGS) $(CFLAGS) -o $@ $< $$($(SDL_CONFIG) $(SDL_FLAGS))

clean:
	rm -f pegsol pegsol.exe

.PHONY: all clean
