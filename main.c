#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"
#include "SDL.h"
#include "peg.h"

static int
locked(const Uint8 *pegs)
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
	return sum;
}

static bool
is_valid_movement(const Uint8 *pegs, int peg, int dest, int *deleted)
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
render(SDL_Renderer *renderer, SDL_Texture *pegt, const Uint8 *pegs, int size, int selected)
{
	int tile = size / 7;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	for (int y = 0; y < 7; y++)
		for (int x = 0; x < 7; x++) {
			if ((y < 2 || y > 4) && (x < 2 || x > 4)) {
				continue;
			}

			SDL_Rect dest;

			int i = y*7 + x;
			if (pegs[y] & 1 << x) {

				dest.w = size/13;
				dest.h = size/13;
				dest.x = x*tile + tile/2-size/26;
				dest.y = y*tile + tile/2-size/26;

				if (i == selected) {
					SDL_SetTextureColorMod(pegt, 0, 255, 0);
				} else {
					SDL_SetTextureColorMod(pegt, 255, 0, 0);
				}
			} else {
				SDL_SetTextureColorMod(pegt, 127, 127, 127);

				dest.w = size/26;
				dest.h = size/26;
				dest.x = x*tile + tile/2-size/52;
				dest.y = y*tile + tile/2-size/52;
			}
			SDL_RenderCopy(renderer, pegt, NULL, &dest);
		}

	SDL_RenderPresent(renderer);
}

static inline void
setup_board(Uint8 *pegs, int *selected)
{
	pegs[0] = 28;  // 0b00011100
	pegs[1] = 28;  // 0b00011100
	pegs[2] = 127; // 0b01111111
	pegs[3] = 119; // 0b01110111
	pegs[4] = 127; // 0b01111111
	pegs[5] = 28;  // 0b00011100
	pegs[6] = 28;  // 0b00011100
	*selected = -1;
}

int
main(int argc, char *argv[])
{
	Uint8 pegs[7];
	int selected;
	int size = 512;

	int option;
	struct optparse options;

	optparse_init(&options, argv);
	while ((option = optparse(&options, "d:")) != -1) {
		switch (option) {
		case 'd':
			size = atoi(options.optarg);
			break;
		case '?':
			SDL_Log("%s: %s\n", argv[0], options.errmsg);
			return 1;
		}
	}

	int tile = size / 7;
	int half_tile = size / 14;
	int radius = size / 26;

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

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == NULL) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR, "SDL_CreateRenderer()", SDL_GetError(), window
		);
		SDL_Log("SDL_CreateRenderer(): %s", SDL_GetError());
		return 1;
	}

	SDL_RWops *bitmap = SDL_RWFromConstMem(peg_bmp, peg_bmp_len);
	SDL_Surface *surface = SDL_LoadBMP_RW(bitmap, 1);
	SDL_FreeRW(bitmap);

	if (surface == NULL)
		return 1;

	SDL_Texture *pegt = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	setup_board(pegs, &selected);

	SDL_Event e;
	bool quit = false;
	while(!quit) {
		render(renderer, pegt, pegs, size, selected);
		SDL_WaitEvent(&e);
		switch (e.type) {
		case SDL_KEYDOWN: 
			switch (e.key.keysym.sym) {
			case 'r':
				setup_board(pegs, &selected);
				break;
			case 'q':
				quit = true;
				break;
			}
			break;
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEBUTTONDOWN:
			int x, y;
			SDL_GetMouseState(&x, &y);

			if (!((y > tile*2 && y < tile*5) || (x > tile*2 && x < tile*5))) {
				selected = -1;
				continue;
			}

			int x_n = x/tile;
			int y_n = y/tile;
			int n = y_n*7 + x_n;

			if (pegs[y_n] & 1 << x_n) {
				int x_s = x -x%tile + half_tile;
				int y_s = y -y%tile + half_tile;
				if (
					(x*x - 2*x*x_s + x_s*x_s) +
					(y*y - 2*y*y_s + y_s*y_s)
					< radius*radius
				) {
					selected = n;
				}
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

			render(renderer, pegt, pegs, size, selected);

			char msg[] = "You Win!\nThere is only 1 peg left!";
			if (res > 1)
				// Don't worry, there will never be an overflow here :)
				sprintf(msg, "You lose\nThere are %d pegs left", res);


			SDL_MessageBoxButtonData btns[] = {
				{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
				0,
				"quit"},
				{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
				1,
				"restart"}
			};

			SDL_MessageBoxData msgbox = {
				SDL_MESSAGEBOX_INFORMATION,
				window,
				"END",
				msg,
				2,
				btns,
				NULL
			};

			int rc;
			SDL_ShowMessageBox(&msgbox, &rc);
			if (rc)
				setup_board(pegs, &selected);
			else
				quit = true;
		}
	}

	SDL_DestroyTexture(pegt);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
