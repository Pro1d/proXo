#include <algorithm>
#include <SDL/SDL.h>
#include "../core/realtime/Buffer.h"
#include "../core/math/basics.h"
#include "wrapper.h"

void bufferToBitmap24bpp(Buffer & buffer, SDL_Surface * bmp, positive sampleSize) {
    SDL_LockSurface(bmp);

    positive W = std::min(buffer.width/sampleSize, (positive) bmp->w);
    positive H = std::min(buffer.height/sampleSize, (positive) bmp->h);
    positive samplePixelsCount = sampleSize*sampleSize;

    for(positive y = 0; y < H; y++)
    for(positive x = 0; x < W; x++)
    {
        real color[4] = {0,0,0,0};
        for(positive by = y*sampleSize; by < (y+1)*sampleSize; by++)
        for(positive bx = x*sampleSize; bx < (x+1)*sampleSize; bx++)
        {
            positive idx = (bx + by * buffer.width) * VEC4_SCALARS_COUNT + 1;
            color[0] += buffer.data[idx+0];
            color[1] += buffer.data[idx+1];
            color[2] += buffer.data[idx+2];
        }
        color[0] /= samplePixelsCount;
        color[1] /= samplePixelsCount;
        color[2] /= samplePixelsCount;
        Uint8 r = clamp01(color[0]) * 255;
        Uint8 g = clamp01(color[1]) * 255;
        Uint8 b = clamp01(color[2]) * 255;


        Uint8 bpp = 3;
        Uint8 *p = (Uint8*) bmp->pixels + y * bmp->pitch + x * bpp;
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = r;
            p[1] = g;
            p[2] = b;
        } else {
            p[0] = b;
            p[1] = g;
            p[2] = r;
        }
    }

    SDL_UnlockSurface(bmp);
}
