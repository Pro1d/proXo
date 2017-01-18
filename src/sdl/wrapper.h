#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include <SDL/SDL.h>
#include "../core/realtime/Buffer.h"
#include "../core/math/basics.h"

void bufferToBitmap24bpp(Buffer & buffer, SDL_Surface * bmp, positive sampleSize);

#endif // __SDL_WRAPPER_H__
