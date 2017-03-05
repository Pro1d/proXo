#include "wrapper.h"
#include "core/config/global.h"
#include <algorithm>
#include <iostream>

using namespace std;

void bufferToBitmap24bpp(Buffer& buffer, SDL_Surface* bmp, positive sampleSize)
{
	SDL_LockSurface(bmp);

	positive sampleSizeExp = 0;
	while(((positive) 1 << sampleSizeExp) < sampleSize)
		sampleSizeExp++;
	sampleSize                    = (1 << sampleSizeExp);
	positive samplePixelsCountExp = sampleSizeExp + sampleSizeExp;

	positive eW = std::min(buffer.width >> sampleSizeExp, (positive) bmp->w)
	    << sampleSizeExp;
	positive eH = std::min(buffer.height >> sampleSizeExp, (positive) bmp->h)
	    << sampleSizeExp;
	Uint8 bpp = 3;
	vec4 data = buffer.data + 1;

	for(positive bsy = 0, bey = sampleSize; bsy < eH;
	    bsy = bey, bey += sampleSize) {
		Uint8* pixel =
		    (Uint8*) bmp->pixels + (bsy >> sampleSizeExp) * bmp->pitch;
		for(positive x = 0; x < eW; x += sampleSize) {
			real color[3] = { 0, 0, 0 };
			for(positive by = bsy; by < bey; by++) {
				positive idx = (by * buffer.width + x) * VEC4_SCALARS_COUNT;
				for(positive bx = 0; bx < sampleSize; bx++) {
					color[0] += data[idx + 0];
					color[1] += data[idx + 1];
					color[2] += data[idx + 2];
					idx += VEC4_SCALARS_COUNT;
				}
			}

			Uint8 r = color[0] <= 0 ?
			    0 :
			    std::min((positive)(color[0] * 255) >> samplePixelsCountExp,
			        (positive) 255);
			Uint8 g = color[1] <= 0 ?
			    0 :
			    std::min((positive)(color[1] * 255) >> samplePixelsCountExp,
			        (positive) 255);
			Uint8 b = color[2] <= 0 ?
			    0 :
			    std::min((positive)(color[2] * 255) >> samplePixelsCountExp,
			        (positive) 255);

			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel[0] = r;
				pixel[1] = g;
				pixel[2] = b;
			}
			else {
				pixel[0] = b;
				pixel[1] = g;
				pixel[2] = r;
			}
			pixel += bpp;
		}
	}

	SDL_UnlockSurface(bmp);
}
void bufferToBitmap24bppOpt(
    Buffer& buffer, SDL_Surface* bmp, positive sampleSize)
{
	SDL_LockSurface(bmp);

	positive sampleSizeExp = 0;
	while(((positive) 1 << sampleSizeExp) < sampleSize)
		sampleSizeExp++;
	sampleSize                          = (1 << sampleSizeExp);
	const positive samplePixelsCountExp = sampleSizeExp + sampleSizeExp;

	const positive eW =
	    (std::min(buffer.width >> sampleSizeExp, (positive) bmp->w)
	        << sampleSizeExp)
	    * VEC4_SCALARS_COUNT;
	const positive eH =
	    (std::min(buffer.height >> sampleSizeExp, (positive) bmp->h)
	        << sampleSizeExp);
	const positive bW          = bmp->w * VEC4_SCALARS_COUNT;
	const positive sS          = sampleSize * VEC4_SCALARS_COUNT;
	const Uint8 bpp            = 3;
	const positive bufferPitch = buffer.width * VEC4_SCALARS_COUNT;

	real line[bmp->w * VEC4_SCALARS_COUNT];
	vec4 endLine     = line + bW;
	vec4 dataLine    = buffer.data + 1;
	Uint8* pixelLine = (Uint8*) bmp->pixels;

	for(positive ey = 0, eyEnd = sampleSize; ey < eH;
	    eyEnd += sampleSize, pixelLine += bmp->pitch) {
		memset(line, 0, bW);

		for(; ey < eyEnd; ey++, dataLine += bufferPitch) {
			const vec4 dataEnd = dataLine + eW;
			for(vec4 data = dataLine, dataSEnd = data + sS, it = line;
			    data < dataEnd; it += VEC4_SCALARS_COUNT, dataSEnd += sS) {
				while(data < dataSEnd) {
					it[0] += data[0];
					it[1] += data[1];
					it[2] += data[2];
					data += VEC4_SCALARS_COUNT;
				}
			}
		}

		Uint8* pixel = pixelLine;
		for(vec4 it = line; it < endLine; it += VEC4_SCALARS_COUNT) {
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel[0] = it[0] <= 0 ?
				    0 :
				    std::min((positive)(it[0] * 255) >> samplePixelsCountExp,
				        (positive) 255);
				pixel[1] = it[1] <= 0 ?
				    0 :
				    std::min((positive)(it[1] * 255) >> samplePixelsCountExp,
				        (positive) 255);
				pixel[2] = it[2] <= 0 ?
				    0 :
				    std::min((positive)(it[2] * 255) >> samplePixelsCountExp,
				        (positive) 255);
			}
			else {
				pixel[0] = it[0] <= 0 ?
				    0 :
				    std::min((positive)(it[0] * 255) >> samplePixelsCountExp,
				        (positive) 255);
				pixel[1] = it[1] <= 0 ?
				    0 :
				    std::min((positive)(it[1] * 255) >> samplePixelsCountExp,
				        (positive) 255);
				pixel[2] = it[2] <= 0 ?
				    0 :
				    std::min((positive)(it[2] * 255) >> samplePixelsCountExp,
				        (positive) 255);
			}

			pixel += bpp;
		}
	}

	SDL_UnlockSurface(bmp);
}

void BufferToBitmap::bufferToBitmap24bppThread(
    void* data, positive threadId, positive threadsCount)
{
	BufferToBitmap* that                  = (BufferToBitmap*) data;
	const positive sampleSize             = that->sampleSize;
	const positive sampleSize_log2        = that->sampleSize_log2;
	const positive samplePixelsCount_log2 = that->samplePixelsCount_log2;
	const Buffer& buffer                  = *that->buffer;
	vec4 bufferData                       = buffer.data + 1;
	SDL_Surface* const bitmap             = that->bitmap;

	const Uint8 bpp = 3;

	const positive bufferWidth =
	    std::min(buffer.width >> sampleSize_log2, (positive) bitmap->w)
	    << sampleSize_log2;
	const positive bitmapHeight =
	    std::min(buffer.height >> sampleSize_log2, (positive) bitmap->h);

	// Get the y range in which this thread will work
	const positive yStart = (threadId * bitmapHeight / threadsCount)
	    << sampleSize_log2;
	const positive yEnd = ((threadId + 1) * bitmapHeight / threadsCount)
	    << sampleSize_log2;

	// Go through each sampled area (vertically)
	for(positive sy = yStart; sy < yEnd; sy += sampleSize) {
		Uint8* pixel =
		    (Uint8*) bitmap->pixels + (sy >> sampleSize_log2) * bitmap->pitch;

		// Go through each sampled area (horizontally)
		for(positive x = 0; x < bufferWidth; x += sampleSize) {
			// Sum of color values in the sampled area of the buffer
			real color[3] = { 0, 0, 0 };
			vec4 bufferDataLine =
			    bufferData + (sy * buffer.width * VEC4_SCALARS_COUNT);
			vec4 bufferDataLineEnd = bufferDataLine
			    + (sampleSize * buffer.width * VEC4_SCALARS_COUNT);
			for(; bufferDataLine < bufferDataLineEnd;
			    bufferDataLine += buffer.width * VEC4_SCALARS_COUNT) {
				vec4 bufferDataPtr = bufferDataLine + (x * VEC4_SCALARS_COUNT);
				vec4 bufferDataPtrEnd =
				    bufferDataPtr + (sampleSize * VEC4_SCALARS_COUNT);

				for(; bufferDataPtr < bufferDataPtrEnd;
				    bufferDataPtr += VEC4_SCALARS_COUNT) {
					color[0] += bufferDataPtr[0];
					color[1] += bufferDataPtr[1];
					color[2] += bufferDataPtr[2];
				}
			}

			// Get mean color values
			Uint8 r = color[0] <= 0 ?
			    0 :
			    std::min((positive)(color[0] * 255) >> samplePixelsCount_log2,
			        (positive) 255);
			Uint8 g = color[1] <= 0 ?
			    0 :
			    std::min((positive)(color[1] * 255) >> samplePixelsCount_log2,
			        (positive) 255);
			Uint8 b = color[2] <= 0 ?
			    0 :
			    std::min((positive)(color[2] * 255) >> samplePixelsCount_log2,
			        (positive) 255);

			// Write in bitmap
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel[0] = r;
				pixel[1] = g;
				pixel[2] = b;
			}
			else {
				pixel[0] = b;
				pixel[1] = g;
				pixel[2] = r;
			}
			pixel += bpp;
		}
	}
}

BufferToBitmap::BufferToBitmap(
    Buffer& buffer, SDL_Surface* bmp, positive sampleSize)
    : multithread(THREADS_COUNT), buffer(&buffer), bitmap(bmp),
      sampleSize(sampleSize)
{
	// Define sampleSize_log2: 2^(sampleSize_log2) <= sampleSize
	sampleSize_log2 = 0;
	while(((positive) 1 << sampleSize_log2) < sampleSize)
		sampleSize_log2++;

	// make sure sampleSize is power of 2
	sampleSize = (1 << sampleSize_log2);

	// binary logarithm of total pixel count in a sample: samplePixelsCount =
	// sampleSize^2
	samplePixelsCount_log2 = sampleSize_log2 * 2;
}

void BufferToBitmap::convert()
{
	SDL_LockSurface(bitmap);

	multithread.execute(
	    BufferToBitmap::bufferToBitmap24bppThread, (void*) this);

	SDL_UnlockSurface(bitmap);
}
