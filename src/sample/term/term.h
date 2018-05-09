#ifndef __TERM_H__
#define __TERM_H__

#include "core/math/basics.h"
#include "core/common/Buffer.h"
#include <memory>

class BufferToXTerm {
public:
  BufferToXTerm(proxo::Buffer& buffer);
  std::shared_ptr<char> convert();
  std::shared_ptr<char> convert2();
  static void getViewSize(proxo::positive& w, proxo::positive& h);
private:
  static proxo::positive bash_color(proxo::vec3 clr);

  static std::shared_ptr<char> createTextBuffer(proxo::positive w, proxo::positive h);
  static std::shared_ptr<char> createIntToStrCache();

  proxo::Buffer& buffer;
  std::shared_ptr<char> intToStrCache;
  std::shared_ptr<char> text;
};

#endif // __TERM_H__
