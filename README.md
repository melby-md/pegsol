# PegSol

Peg Solitaire written in C99 and SDL2.

## Requirements

If you're on Windows (I will eventually add msvc support):

 - MinGW-w64 and GNU Make (I recomend getting it with [w64devkit](https://github.com/skeeto/w64devkit))
 - SDL2 (download the mingw version and add the `SDL2-x.x.x\x86_64-w64-mingw32\bin` folder to your path)

If you're on \*NIX:

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

to build a Windows executable from Windows:

    make pegsol.exe

to cross-compile the Windows executable:

    make CROSS="x86_64-w64-mingw32-" pegsol.exe

## CLI

You can adjust the window size in pixels in the command line:

    ./pegsol -d 600

## License

Copyright 2023 Pedro Debs &lt;<pedrodebs1@gmail.com>&gt;

This software is released under the 0BSD license unless otherwise noted (see the
`LICENSE` file or <https://spdx.org/licenses/0BSD.html>).

The file `optparse.h` was obtained in <https://github.com/skeeto/optparse>
and is free and unencumbered software released into the public domain.

