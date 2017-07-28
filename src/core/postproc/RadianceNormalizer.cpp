#include "core/postproc/RadianceNormalizer.h"
#include "core/math/basics.h"
#include <cmath>

namespace proxo {

RadianceNormalizer::RadianceNormalizer(MultiThread& multithread, real radianceMeanTarget, real maxFactor) :
  multithread_(multithread),
  radianceMeanTarget_(radianceMeanTarget),
  maxScale_(maxFactor),
  minScale_(1 / maxFactor)
{
}

void RadianceNormalizer::normalize(Buffer & buffer)
{
  real mean = computeAverageIntensity(buffer);
  real scale = radianceMeanTarget_ / mean;
  real effectiveScale = (scale - 1) * (scale - 1) * (scale - 1) + 1;
  if(scale > 1)
    scale = 1; //log(scale);
  else
    scale = 1 / (log(1/scale));
  effectiveScale = scale;
  //real effectiveScale = clamp(scale, minScale_, maxScale_);

  applyScale(buffer, effectiveScale);
}

real RadianceNormalizer::computeAverageIntensity(Buffer & buffer)
{
  real sum = 0;

  positive pixelCount = buffer.width*buffer.height;
  vec4 end = buffer.data + pixelCount * VEC4_SCALARS_COUNT;
  for(vec4 ptr = buffer.data; ptr != end; ptr += VEC4_SCALARS_COUNT) {
    sum += (ptr[BUF_R_OFFSET] + ptr[BUF_G_OFFSET] + ptr[BUF_B_OFFSET]);
  }

  return sum / (3 * pixelCount);
}

void RadianceNormalizer::applyScale(Buffer & buffer, real scale)
{
  positive pixelCount = buffer.width*buffer.height;
  vec4 end = buffer.data + pixelCount * VEC4_SCALARS_COUNT;
  for(vec4 ptr = buffer.data; ptr != end; ptr += VEC4_SCALARS_COUNT) {
    ptr[BUF_R_OFFSET] *= scale;
    ptr[BUF_G_OFFSET] *= scale;
    ptr[BUF_B_OFFSET] *= scale;
  }
}

} // namespace proxo
