#include "reference.h"

#include <SDL2/SDL.h>

#include <vector>
#include <cassert>
#include <iostream>
#include <stdexcept>

const unsigned width = 1366;
const unsigned height = 768;
const unsigned bpp = 32;
static SDL_Texture* texture_ = NULL;
enum Color {
	WHITE,BLACK
};

static uint32_t
my_rand()
{
	static uint32_t state = 0;
	return (state = 22695477u * state + 1u) >> 23;
}

static void
plot(SDL_Surface* surface, unsigned x, unsigned y)
{
	SDL_Rect rec;
	rec.x = x;
	rec.y = y;
	rec.w = 2;
	rec.h = 2;
	SDL_FillRect(surface,&rec,SDL_MapRGB(surface->format,0xff,0xff,0xff));
}

class Arena {
public:
	Arena(unsigned width, unsigned height)
	: w_(width)
	, h_(height)
	, image_(width * height)
	, next_(width * height)
	{
		for (Image::iterator i(image_.begin()); i != image_.end(); ++i) {
			*i = my_rand() & 1;
		}
	}

	void
	draw(SDL_Renderer* ren)
	{
		SDL_RenderClear(ren);
		SDL_Surface* surface = SDL_CreateRGBSurface(0,width,height,bpp,0,0,0,100);
		SDL_LockSurface(surface);
		for (unsigned y = 0; y < h_; ++y) {
			for (unsigned x = 0; x < w_; ++x) {
				Color c = at(x, y) ? WHITE : BLACK;
				if(c==WHITE)	plot(surface, x*2, y*2);
			}
		}
		SDL_Rect destination;
		destination.x = 0;
		destination.y = 0;
		SDL_UnlockSurface(surface);
		if(texture_ != NULL) {SDL_DestroyTexture(texture_);}
		texture_ = SDL_CreateTextureFromSurface(ren,surface);
		SDL_QueryTexture(texture_, NULL, NULL, &destination.w, &destination.h);
		SDL_RenderCopy(ren,texture_,NULL,&destination);
		SDL_FreeSurface(surface);
	}

	uint8_t&
	at(unsigned x, unsigned y)
	{
		assert(x < w_);
		assert(y < h_);
		return image_.at(w_ * y + x);
	}

	uint8_t&
	atNext(unsigned x, unsigned y)
	{
		assert(x < w_);
		assert(y < h_);
		return next_.at(w_ * y + x);
	}

	unsigned
	countNeighs(unsigned x, unsigned y)
	{
		unsigned x1 = (x + w_ - 1) % w_;
		unsigned y1 = (y + h_ - 1) % h_;
		unsigned x2 = (x + 1) % w_;
		unsigned y2 = (y + 1) % h_;
		return at(x1, y1) + at(x, y1) + at(x2, y1) + at(x1, y) + at(x2, y) + at(x1, y2) + at(x, y2) + at(x2, y2);
	}

	void
	update()
	{
		for (unsigned y = 0; y < h_; ++y) {
			for (unsigned x = 0; x < w_; ++x) {
				bool live = at(x, y);
				unsigned neighCount = countNeighs(x, y);
				if (live) {
					if (neighCount < 2 || neighCount > 3) {
						live = false;
					}
				} else if (neighCount == 3) {
					live = true;
				}
				atNext(x, y) = live;
			}
		}
		image_.swap(next_);
	}

	bool check() const
	{
		const short* p = reference_rle;
		Image::const_iterator i(image_.begin());
		int state = *i;
		int count = 1;
		if (*p++ != state) return false;
		for (++i; i != image_.end(); ++i) {
			if (*i == state) {
				++count;
			} else {
				if (*p++ != count) return false;
				state = *i;
				count = 1;
			}
		}
		return *p == count;
	}

private:
	typedef std::vector<uint8_t> Image;

	unsigned w_, h_;
	Image image_, next_;
};

int
main()
{
	Arena a(width / 2, height / 2);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		throw std::runtime_error(SDL_GetError());
	}
	SDL_Window* win = SDL_CreateWindow("gameoflife", 0, 0, width, height, SDL_WINDOW_FULLSCREEN);
	if (win == 0) {
		throw std::runtime_error(SDL_GetError());
	}
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == 0) {
		throw std::runtime_error(SDL_GetError());
	}

	bool running = true;
	unsigned genCount = 0;
	const Uint32 t0 = SDL_GetTicks();
	while (running && genCount < 2000) {
		SDL_Event ev;
		a.update();
		++genCount;
		a.draw(ren);
		SDL_RenderPresent(ren);
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
			case SDL_KEYDOWN:
				running = false;
				break;
			}
		}
	}
	const Uint32 t1 = SDL_GetTicks();
	const unsigned tt = t1 - t0;
	const float gps = float(genCount) * 1000.0f / tt;
	std::cerr << genCount << " generations, " << tt << " ms => " << gps << " GPS" << std::endl;
	SDL_Quit();
	if (a.check()) {
		std::cerr << "OK" << std::endl;
		return EXIT_SUCCESS;
	} else {
		std::cerr << "Failure! :(" << std::endl;
		return EXIT_FAILURE;
	}
}
