#include "reference.h"

#include <SDL/SDL.h>

#include <vector>
#include <cassert>
#include <iostream>

const unsigned width = 1280;
const unsigned height = 800;
const unsigned bpp = 32;
static Uint32 white;
static Uint32 black;

static uint32_t
my_rand()
{
	static uint32_t state = 0;
	return (state = 22695477u * state + 1u) >> 23;
}

static void
plot(SDL_Surface* screen, unsigned x, unsigned y, Uint32 c)
{
	if (x >= width || y >= height) return;
	Uint32* pix = reinterpret_cast<Uint32*>(screen->pixels) + width * y + x;
	*pix = c;
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
	draw(SDL_Surface* screen)
	{
		for (unsigned y = 0; y < h_; ++y) {
			for (unsigned x = 0; x < w_; ++x) {
				Uint32 c = at(x, y) ? white : black;
//				plot(screen, x, y, c);
				plot(screen, x * 2, y * 2, c);
				plot(screen, x * 2 + 1, y * 2, c);
				plot(screen, x * 2, y * 2 + 1, c);
				plot(screen, x * 2 + 1, y * 2 + 1, c);
			}
		}
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
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* screen = SDL_SetVideoMode(width, height, bpp, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
	assert(screen);
	white = SDL_MapRGB(screen->format, 255, 255, 255);
	black = SDL_MapRGB(screen->format, 0, 0, 0);
	bool running = true;
	unsigned genCount = 0;
	const Uint32 t0 = SDL_GetTicks();
	while (running && genCount < 2000) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
			case SDL_KEYDOWN:
				running = false;
				break;
			}
		}
//		SDL_FillRect(screen, 0, black);
		a.update();
		++genCount;
		a.draw(screen);
		SDL_Flip(screen);
//		SDL_Delay(40);
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
