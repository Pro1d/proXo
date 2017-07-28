#ifndef __RADIANCE_NORMALIZER_H__
#define __RADIANCE_NORMALIZER_H__

#include "core/common/Buffer.h"
#include "core/sys/Multithreading.h"
#include "core/math/type.h"

namespace proxo {

class RadianceNormalizer {
public:
  RadianceNormalizer(MultiThread& multithread, real radianceMeanTarget = 0.75, real maxFactor = 2);
  void normalize(Buffer & buffer);
  real computeAverageIntensity(Buffer & buffer);
  void applyScale(Buffer & buffer, real scale);
private:
  MultiThread& multithread_;
  real radianceMeanTarget_;
  real maxScale_, minScale_;
};

} // namespace proxo

#endif // __RADIANCE_NORMALIZER_H__
