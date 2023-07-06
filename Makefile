CC = $(CROSS)cc -std=c99
WINDRES = $(CROSS)windres
SDL_CONFIG = sdl2-config
SDL_FLAGS = --cflags --libs
WARNINGS = -Wall -Wextra -Wvla -Wno-unused-parameter -Wno-unused-function

all: pegsol

pegsol: main.c optparse.h peg.h
	$(CC) $(WARNINGS) $(CFLAGS) -o $@ $< $$($(SDL_CONFIG) $(SDL_FLAGS))

pegsol.exe: main.c icon.o optparse.h peg.h
	$(CC) $(WARNINGS) $(CFLAGS) -o $@ main.c icon.o $$($(SDL_CONFIG) $(SDL_FLAGS))

icon.o: peg.ico
	echo '1 ICON "peg.ico"' | $(WINDRES) -o $@

clean:
	rm -f pegsol pegsol.exe icon.o

.PHONY: all clean
