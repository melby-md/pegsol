CC = $(CROSS)cc
WINDRES = $(CROSS)windres
WARN = -Wall -Wextra -Wvla -pedantic

SDL_CONFIG = sdl2-config

SDL_LIB_ = --libs
SDL_LIB_0 = --libs
SDL_LIB_1 = --static-libs
SDL_LIB = $(SDL_LIB_$(STATIC_SDL))

Windows_NT = .exe

all: pegsol$($(OS))

pegsol: main.c peg.h
	$(CC) -std=c99 $(WARN) $(CFLAGS) $(LDFLAGS) -o $@ $< $$($(SDL_CONFIG) --cflags $(SDL_LIB))

pegsol.exe: main.o icon.o
	$(CC) $(WARN) $(LDFLAGS) -o $@ main.o icon.o $$($(SDL_CONFIG) $(SDL_LIB))

icon.o: peg.ico
	echo '1 ICON "peg.ico"' | $(WINDRES) -o $@

main.o: main.c  peg.h
	$(CC) -std=c99 $(WARN) $(CFLAGS) -c -o $@ $< $$($(SDL_CONFIG) --cflags)

clean:
	rm -f pegsol pegsol.exe icon.o main.o

.PHONY: all clean
