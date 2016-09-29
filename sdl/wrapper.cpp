#include <algorithm>
#include <SDL/SDL.h>
#include "../core/realtime/Buffer.h"
#include "../core/math/basics.h"
#include "wrapper.h"

void bufferToBitmap24bpp(Buffer & buffer, SDL_Surface * bmp, positive sampleSize) {
    SDL_LockSurface(bmp);

    positive sampleSizeExp = 0;
    while((1 << sampleSizeExp) < sampleSize)
        sampleSizeExp++;
    sampleSize = (1 << sampleSizeExp);
    positive samplePixelsCountExp = sampleSizeExp+sampleSizeExp;

    positive eW = std::min(buffer.width >> sampleSizeExp, (positive) bmp->w) << sampleSizeExp;
    positive eH = std::min(buffer.height >> sampleSizeExp, (positive) bmp->h) << sampleSizeExp;
    Uint8 bpp = 3;
    vec4 data = buffer.data+1;

    for(positive bsy = 0, bey = sampleSize; bsy < eH; bsy = bey, bey += sampleSize) {
        Uint8 * pixel = (Uint8*) bmp->pixels + (bsy>>sampleSizeExp)*bmp->pitch;
        for(positive x = 0; x < eW; x += sampleSize)
        {
            real color[3] = {0,0,0};
            for(positive by = bsy; by < bey; by++) {
                positive idx = (by * buffer.width + x) * VEC4_SCALARS_COUNT;
                for(positive bx = 0; bx < sampleSize; bx++)
                {
                    color[0] += data[idx+0];
                    color[1] += data[idx+1];
                    color[2] += data[idx+2];
                    idx += VEC4_SCALARS_COUNT;
                }
            }

            Uint8 r = color[0] <= 0 ? 0 : std::min((positive) (color[0] * 255) >> samplePixelsCountExp, (positive) 255);
            Uint8 g = color[1] <= 0 ? 0 : std::min((positive) (color[1] * 255) >> samplePixelsCountExp, (positive) 255);
            Uint8 b = color[2] <= 0 ? 0 : std::min((positive) (color[2] * 255) >> samplePixelsCountExp, (positive) 255);

            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel[0] = r;
                pixel[1] = g;
                pixel[2] = b;
            } else {
                pixel[0] = b;
                pixel[1] = g;
                pixel[2] = r;
            }
            pixel += bpp;
        }
    }

    SDL_UnlockSurface(bmp);
}
