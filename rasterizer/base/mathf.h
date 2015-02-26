#ifndef _BASE_MATHF_H_
#define _BASE_MATHF_H_

#include "header.h"

namespace rasterizer
{

class Mathf
{
public:
	static const float epsilon;
	static const float inifinity;
	static const float PI;
	static const float deg2rad;
	static const float rad2deg;

	static float Abs(float f);
	static float Sign(float f);
	static bool Approximately(float a, float b);

	static float Floor(float f);
	static int FloorToInt(float f);
	static float Ceil(float f);
	static int CeilToInt(float f);
	static float Round(float f);
	static int RoundToInt(float f);

	static int Clamp(int value, int min, int max);
	static float Clamp(float value, float min, float max);
	static float Clamp01(float value);

	static float Min(float a, float b);
	static float Min(const float* values, int count);
	static float Min(const std::vector<float>& values);
	static float Max(float a, float b);
	static float Max(const float* values, int count);
	static float Max(const std::vector<float>& values);

	static float Sqrt(float value);
	static float Pow(float f, float p);
	static float Exp(float f);
	static float Log(float f);
	static float Log10(float f);

	static bool IsPowerOfTwo(int value);
	static int NextPowerOfTwo(int value);

	static float Sin(float f);
	static float Cos(float f);
	static float Tan(float f);
	static float Asin(float f);
	static float Acos(float f);
	static float Atan(float f);
	static float Atan2(float y, float x);

};

} // namespace rasterizer

#endif //!_BASE_MATHF_H_
