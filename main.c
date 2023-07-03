#include <stdbool.h>
#include <stdint.h>

#include "SDL.h"

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
locked(const uint8_t *pegs)
{
	int a, b, c;
	int sum = 0;

	for (int l = 0; l < 7; l ++) {
		if (l > 1 && l < 5) {
			for (int i = 0; i < 5; i++) {
				a = (pegs[l] >> (2 + i)) & 1;
				b = (pegs[l] >> (1 + i)) & 1;
				c = (pegs[l] >> i) & 1;
				if (b && (b || c) && (a != c)) {
					return 0;
				}
			}
		} else {
			a = (pegs[l] >> 4) & 1;
			b = (pegs[l] >> 3) & 1;
			c = (pegs[l] >> 2) & 1;
			if (b && (b || c) && (a != c)) {
				return 0;
			}
		}
	}

	for (int l = 0; l < 7; l ++) {
		if (l > 1 && l < 5) {
			sum += ((pegs[1] >> l) & 1) + ((pegs[0] >> l) & 1);
			for (int i = 0; i < 5; i++) {
				a = (pegs[i + 2] >> l) & 1;
				b = (pegs[i + 1] >> l) & 1;
				c = (pegs[i] >> l) & 1;
				if (b && (b || c) && (a != c)) {
					return 0;
				}
				sum += a;
			}
		} else {
			a = (pegs[4] >> l) & 1;
			b = (pegs[3] >> l) & 1;
			c = (pegs[2] >> l) & 1;
			if (b && (b || c) && (a != c)) {
				return 0;
			}
			sum += a + b + c;
		}
	}
	if (sum == 1)
		return -1;
	else
		return 1;
}

static bool
is_valid_movement(const uint8_t *pegs, int peg, int dest, int *deleted)
{
	*deleted = -1;

	switch (dest - peg) {
	case 2:
		*deleted = peg + 1;
		break;
	case -2:
		*deleted = peg - 1;
		break;
	case 14:
		*deleted = peg + 7;
		break;
	case -14:
		*deleted = peg - 7;
	}

	if (*deleted < 0)
		return false;

	if (pegs[*deleted/7] & 1 << *deleted%7)
		return true;

	return false;
}

static void
render(SDL_Renderer *renderer, const uint8_t *pegs, int size, int selected)
{
	int tile = size / 7;
	int half_tile = size / 14;
	int radius = size / 26;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 7; x++) {
			if ((y < 2 || y > 4) && (x < 2 || x > 4)) {
				continue;
			}

			int i = y*7 + x;
			int rad = radius;
			if (pegs[y] & 1 << x) {
				if (i == selected)
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				else
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
				rad /= 2;
			}
			draw_circle(renderer, x*tile+half_tile, y*tile+half_tile, rad);
		}

	SDL_RenderPresent(renderer);
}

int
main(int argc, char *args[])
{
	uint8_t pegs[7] = {28, 28, 127, 119, 127, 28, 28};
	int size = 512;
	int tile = size / 7;
	int half_tile = size / 14;
	int radius = size / 26;
	int selected = -1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_Init()", SDL_GetError(), 0
		);
		SDL_Log("SDL_Init(): %s", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"Peg Solitaire",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size,
		size,
		SDL_WINDOW_SHOWN
	);

	if (window == NULL) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow()", SDL_GetError(), 0
		);
		SDL_Log("SDL_CreateWindow(): %s", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_CreateRenderer()", SDL_GetError(), window
		);
		SDL_Log("SDL_CreateRenderer(): %s", SDL_GetError());
		return 1;
	}

	SDL_Event e;
	bool quit = false;
	while(!quit) {
		render(renderer, pegs, size, selected);
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				int x, y, x_s, y_s, n;
				SDL_GetMouseState(&x, &y);

				if (!((y > tile*2 && y < tile*5) || (x > tile*2 && x < tile*5))) {
					selected = -1;
					continue;
				}

				x_s = x -x%tile + half_tile;
				y_s = y -y%tile + half_tile;
				if ((x*x - 2*x*x_s + x_s*x_s) + (y*y - 2*y*y_s + y_s*y_s) >= radius*radius) {
					selected = -1;
					continue;
				}

				int x_n = x/tile;
				int y_n = y/tile;
				n = y_n*7 + x_n;

				if (pegs[y_n] & 1 << x_n) {
					selected = n;
					continue;
				}

				int deleted;
				if (!is_valid_movement(pegs, selected, n, &deleted)) {
					selected = -1;
					continue;
				}

				pegs[deleted/7] &= ~(1 << deleted%7);
				pegs[selected/7] &= ~(1 << selected%7);
				pegs[n/7] |= 1 << n%7;

				int res;
				if (!(res = locked(pegs))) {
					continue;
				}

				render(renderer, pegs, size, selected);
				char *msg;
				if (res == -1)
					msg = "You Win!";
				else
					msg = "You Lose";

				SDL_ShowSimpleMessageBox(
					SDL_MESSAGEBOX_INFORMATION,
					"END",
					msg,
					window	
				);
				quit = true;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
