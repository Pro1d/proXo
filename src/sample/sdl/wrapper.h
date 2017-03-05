#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "core/math/basics.h"
#include "core/realtime/Buffer.h"
#include "core/sys/Multithreading.h"
#include <SDL/SDL.h>

void bufferToBitmap24bpp(Buffer& buffer, SDL_Surface* bmp, positive sampleSize);
void bufferToBitmap24bppOpt(
    Buffer& buffer, SDL_Surface* bmp, positive sampleSize);

class BufferToBitmap {
public:
	BufferToBitmap(Buffer& buffer, SDL_Surface* bmp, positive sampleSize);
	void convert();

private:
	static void bufferToBitmap24bppThread(
	    void* data, positive threadId, positive threadsCount);

	MultiThread multithread;
	Buffer* buffer;
	SDL_Surface* bitmap;
	positive sampleSize;
	positive sampleSize_log2;
	positive samplePixelsCount_log2;
};

#endif // __SDL_WRAPPER_H__
