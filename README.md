# PegSol

Peg Solitaire written in C99 and SDL2.

## Requirements

 - Make
 - A C99 compiler
 - SDL2

## Building

Just run:

    make

if sdl2-config isn't in your path:

    make SDL_CONFIG=/path/to/sdl2-config

to use pkg-config instead:

    make SDL_CONFIG="pkg-config sdl2"

to change sdl2-config flags:

    make SDL_FLAGS="--cflags --static-libs"

## License

Copyright 2023 Pedro Debs &lt;<pedrodebs1@gmail.com>&gt;

Licensed under the 0BSD license (see the `LICENSE` file).

