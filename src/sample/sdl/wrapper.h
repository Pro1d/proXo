#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "core/math/basics.h"
#include "core/realtime/Buffer.h"
#include "core/sys/Multithreading.h"
#include <SDL/SDL.h>

void bufferToBitmap24bpp(proxo::Buffer& buffer, SDL_Surface* bmp, proxo::positive sampleSize);
void bufferToBitmap24bppOpt(
    proxo::Buffer& buffer, SDL_Surface* bmp, proxo::positive sampleSize);

class BufferToBitmap {
public:
	BufferToBitmap(proxo::Buffer& buffer, SDL_Surface* bmp, proxo::positive sampleSize);
	void convert();

private:
	static void bufferToBitmap24bppThread(
	    void* data, proxo::positive threadId, proxo::positive threadsCount);

	proxo::MultiThread multithread;
	proxo::Buffer* buffer;
	SDL_Surface* bitmap;
	proxo::positive sampleSize;
	proxo::positive sampleSize_log2;
	proxo::positive samplePixelsCount_log2;
};

#endif // __SDL_WRAPPER_H__
