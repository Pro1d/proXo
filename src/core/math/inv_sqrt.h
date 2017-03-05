#ifndef __FUNC_H__
#define __FUNC_H__

#include <cmath>
#include <cstdint>
#include "core/config/global.h"

namespace proxo {

double sqrt_inv(double x);
float sqrt_inv(float x);

#if INV_SQRT_METHOD == DEFAULT_MATH_INV_SQRT

inline double sqrt_inv(double x)
{
	return 1.0 / sqrt(x);
}

inline float sqrt_inv(float x)
{
	return 1.0f / sqrt(x);
}

#elif INV_SQRT_METHOD == FASTEST_INV_SQRT

inline double sqrt_inv(double x)
{
	int64_t xi = *(int64_t*) &x;
	xi         = 0x5fe6eb50c7b537a9 - (xi >> 1);
	return *(double*) &xi;
}

inline float sqrt_inv(float x)
{
	int32_t xi = *(int32_t*) &x;
	xi         = 0x5f375a86 - (xi >> 1);
	return *(float*) &xi;
}

#elif INV_SQRT_METHOD == NEWTON_INV_SQRT

inline double sqrt_inv(double x)
{
	double half_x = x * 0.5;
	int64_t xi    = *(int64_t*) &x;
	xi            = 0x5fe6eb50c7b537a9 - (xi >> 1);
	x             = *(double*) &xi;
	return x * (1.5 - half_x * x * x);
}

inline float sqrt_inv(float x)
{
	float half_x = x * 0.5f;
	int32_t xi   = *(int32_t*) &x;
	xi           = 0x5f375a86 - (xi >> 1);
	x            = *(float*) &xi;
	return x * (1.5f - half_x * x * x);
}

#elif INV_SQRT_METHOD == NEWTON2_INV_SQRT

inline double sqrt_inv(double x)
{
	double half_x = x * 0.5;
	int64_t xi    = *(int64_t*) &x;
	xi            = 0x5fe6eb50c7b537a9 - (xi >> 1);
	x = *(double*) & xi x = x * (1.5 - half_x * x * x);
	return x * (1.5 - half_x * x * x);
}

inline float sqrt_inv(float x)
{
	float half_x = x * 0.5f;
	int32_t xi   = *(int32_t*) &x;
	xi           = 0x5f375a86 - (xi >> 1);
	x            = *(float*) &xi;
	x            = x * (1.5f - half_x * x * x);
	return x * (1.5f - half_x * x * x);
}

#elif INV_SQRT_METHOD == MULT_NEWTON_INV_SQRT

inline double sqrt_inv(double x)
{
	double half_x = x * 0.5;
	int64_t xi    = *(int64_t*) &x;
	xi            = 0x5fe6eb50c7b537a9 - (xi >> 1);
	x             = *(double*) &xi;
	return x * (1.5 * 1.000363245811462 - half_x * x * x);
}

inline float sqrt_inv(float x)
{
	float half_x = x * 0.5f;
	int32_t xi   = *(int32_t*) &x;
	xi           = 0x5f375a86 - (xi >> 1);
	x            = *(float*) &xi;
	return x * (1.5f * 1.000363245811462f - half_x * x * x);
}

#endif

} // namespace proxo

#endif
