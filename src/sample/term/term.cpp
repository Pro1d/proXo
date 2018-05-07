#include "term.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>


BufferToXTerm::BufferToXTerm(proxo::Buffer& buffer)
  : buffer(buffer),
  intToStrCache(createIntToStrCache()),
  text(createTextBuffer(buffer.width, buffer.height))
{

}

proxo::positive BufferToXTerm::bash_color(proxo::vec3 clr)
{
  return 16
          + (proxo::positive) (clr[0] * 5 + .5f) * 36
          + (proxo::positive) (clr[1] * 5 + .5f) * 6
          + (proxo::positive) (clr[2] * 5 + .5f);
}

std::shared_ptr<char> BufferToXTerm::createTextBuffer(proxo::positive w, proxo::positive h)
{
  const char* header = "\e[H";
  proxo::positive header_len = strlen(header);
  const char* data = "\e[38;5;000m\e[48;5;000m\u2580";
  //                  |-+++|+++|+++-|+++|+++|++-+-|-
  proxo::positive data_len = strlen(data);
  const char* nl = "\e[0m\n";
  proxo::positive nl_len = strlen(nl);

  char* buf = new char[header_len + (w * data_len + nl_len) * h / 2 + 1];
  char* cursor = buf;

  memcpy(cursor, header, header_len);
  cursor += header_len;
  
  for(proxo::positive y = 0; y < h; y += 2) {
    for(proxo::positive x = 0; x < w; x++) {
      memcpy(cursor, data, data_len);
      cursor += data_len;
    }
    memcpy(cursor, nl, nl_len);
    cursor += nl_len;
  }
  *cursor = '\0';
  return std::shared_ptr<char>{buf};
}

std::shared_ptr<char> BufferToXTerm::createIntToStrCache()
{
  char* cache = new char[256*4];
  char* cursor = cache;

  for(proxo::positive i = 0; i < 256; i++, cursor += 4)
    sprintf(cursor, "%03d", i);

  return std::shared_ptr<char>{cache};
}

std::shared_ptr<char> BufferToXTerm::convert()
{
  proxo::positive w = buffer.width;
  proxo::positive h = buffer.height;
  char* cursor = text.get() + 3 + 7;
  proxo::vec4 pxlUp = buffer.data + 1;
  proxo::vec4 pxlDown = buffer.data + VEC4_SCALARS_COUNT * w + 1;
  for(proxo::positive y = 0; y < h; y += 2) {
    for(proxo::positive x = 0; x < w; x++, pxlUp += VEC4_SCALARS_COUNT, pxlDown += VEC4_SCALARS_COUNT) {
      proxo::positive c0 = bash_color(pxlUp);
      proxo::positive c1 = bash_color(pxlDown);
      memcpy(cursor, intToStrCache.get() + 4*c0, 3);
      cursor += 11;
      memcpy(cursor, intToStrCache.get() + 4*c1, 3);
      cursor += 14;
    }
    pxlUp += VEC4_SCALARS_COUNT * w;
    pxlDown += VEC4_SCALARS_COUNT * w;
    cursor += 5;
  }
  *cursor = '\0';
  return text; // fputs(text.get(), stdout);
}

void BufferToXTerm::getViewSize(proxo::positive& w, proxo::positive& h)
{
  struct winsize size;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
  w = size.ws_col;
  h = size.ws_row * 2;
}

