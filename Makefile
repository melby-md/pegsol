CC = $(CROSS)cc -std=c99
WINDRES = $(CROSS)windres
WARN = -Wall -Wextra -Wvla -Wno-unused-parameter -Wno-unused-function

SDL_CONFIG = sdl2-config
SDL_LIB = --static-libs
SDL_LIB$(STATIC_SDL) = --libs

Windows_NT = .exe

all: pegsol$($(OS))

pegsol: main.c optparse.h peg.h
	$(CC) $(WARN) $(CFLAGS) $(LDFLAGS) -o $@ $< $$($(SDL_CONFIG) --cflags $(SDL_LIB)) -lSDL2_ttf

pegsol.exe: main.o icon.o
	$(CC) $(WARN) $(LDFLAGS) -o $@ main.o icon.o $$($(SDL_CONFIG) $(SDL_LIB))

icon.o: peg.ico
	echo '1 ICON "peg.ico"' | $(WINDRES) -o $@

main.o: main.c optparse.h peg.h
	$(CC) $(WARN) $(CFLAGS) -c -o $@ $< $$($(SDL_CONFIG) --cflags)

clean:
	rm -f pegsol pegsol.exe icon.o main.o

.PHONY: all clean
