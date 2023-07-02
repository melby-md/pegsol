#include <stdbool.h>

#include "SDL.h"

// window dimension
static const int size = 512;
static const int tile = size / 7;
static const int half_tile = size / 14;
static const int radius = size / 26;

// https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
static int
draw_circle(SDL_Renderer *renderer, int x, int y, int rad)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = rad;
    d = rad -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (rad - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

static int
offset(int peg)
{
	if (peg > 29)
		return peg + 14;
	else if (peg > 26)
		return peg + 10;
	else if (peg > 5)
		return peg + 8;
	else if (peg > 2)
		return peg + 6;
	return peg + 2;
}

static bool
is_valid_movement(bool *pegs, int peg, int dest, int *deleted)
{
	int p = offset(peg);
	int d = offset(dest);

	switch (d - p) {
	case 2:
		*deleted = peg + 1;
		return true;
	case -2:
		*deleted = peg -1;
		return true;
	case 14:
		if (peg < 3) {
			*deleted = peg + 3;
		} else if (peg < 6 || dest > 29) {
			*deleted = peg + 5;
		} else {
			*deleted = peg + 7;
		}
		return true;
	case -14:
		if (peg > 29) {
			*deleted = peg - 3;
		} else if (peg > 26 || dest < 3) {
			*deleted = peg - 5;
		} else {
			*deleted = peg - 7;
		}
		return true;
	}

	return false;
}

int
main(int argc, char *args[])
{
	bool pegs[33];
	int selected = -1;

	for(int i = 0; i < 33; i++)
		pegs[i] = 1;
	pegs[16] = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_Init()", SDL_GetError(), 0
		);
		SDL_Log("SDL_Init(): %s", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"Solitaire Peg",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size,
		size,
		SDL_WINDOW_SHOWN
	);

	if (window == NULL) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_Init()", SDL_GetError(), 0
		);
		SDL_Log("SDL_Init(): %s", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_Init()", SDL_GetError(), 0
		);
		SDL_Log("SDL_Init(): %s", SDL_GetError());
		return 1;
	}

	SDL_Event e;
	bool quit = false;
	while(!quit) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		int i = 0;
		for (int y = 0; y <= 6; y++)
			for (int x = 0; x <= 6; x++) {
				if ((y > 1 && y < 5) || (x > 1 && x <5)) {
					int rad = radius;
					if (pegs[i]) {
						if (i == selected)
							SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
						else
							SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					} else {
						SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
						rad /= 2;
					}
					draw_circle(renderer, x*tile+half_tile, y*tile+half_tile, rad);
					i++;
				}
			}
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				int x, y, x_s, y_s, n;
				SDL_GetMouseState(&x, &y);
				if ((y > tile*2 && y < tile*5) || (x > tile*2 && x < tile*5)) {
					x_s = x -x%tile + half_tile;
					y_s = y-y%tile + half_tile;
					if ((x*x - 2*x*x_s + x_s*x_s) + (y*y - 2*y*y_s + y_s*y_s) < radius*radius) {
						if (y > tile*2 && y < tile*5)
							n = 6 + x/tile + (y/tile-2)*7;
						else
							n = x/tile + (y/tile)*3 -2 + (y >= tile*5)*12;

						if (pegs[n]) {
							selected = n;
							continue;
						}

						int deleted;
						if (is_valid_movement(pegs, selected, n, &deleted)) {
							pegs[deleted] = 0;
							pegs[selected] = 0;
							pegs[n] = 1;
						} else
							selected = -1;

					} else
						selected = -1;
				} else
					selected = -1;
			}
		}
	}

	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
