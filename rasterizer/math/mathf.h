#ifndef _BASE_MATHF_H_
#define _BASE_MATHF_H_

#include "base/header.h"

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
	static int Abs(int i);
	static float Sign(float f);
	static bool Approximately(float a, float b);

	static float Floor(float f);
	static int FloorToInt(float f);
	static float Ceil(float f);
	static int CeilToInt(float f);
	static float Round(float f);
	static int RoundToInt(float f);

	template<class T>
	static T Clamp(T value, T min, T max);
	static float Clamp01(float value);
	static float Repeat(float t, float length);
	static float PingPong(float t, float length);

	template<class T>
	static T Min(T a, T b);
	template<class T>
	static T Min(T a, T b, T c);
	template<class T>
	static T MinElement(const T* values, int count);
	template<class T>
	static T MinElement(const std::vector<T>& values);
	template<class T>
	static T Max(T a, T b);
	template<class T>
	static T Max(T a, T b, T c);
	template<class T>
	static T MaxElement(const T* values, int count);
	template<class T>
	static T MaxElement(const std::vector<T>& values);

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

#include "mathf.inl"

} // namespace rasterizer

#endif //!_BASE_MATHF_H_
