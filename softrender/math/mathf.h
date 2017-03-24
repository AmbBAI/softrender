#ifndef _MATH_MATHF_H_
#define _MATH_MATHF_H_

#include "base/header.h"

namespace sr
{

class Mathf
{
public:
	static const float epsilon;
	static const float inifinity;
	static const float PI;
	static const float invPI;
	static const float deg2rad;
	static const float rad2deg;

	static inline float Abs(float f);
	static inline int Abs(int i);
	static inline float Sign(float f);
	static inline bool Approximately(float a, float b);

	static inline float Trunc(float f);
	static inline int TruncToInt(float f);
	static inline float Fractional(float f);
	static inline float Floor(float f);
	static inline int FloorToInt(float f);
	static inline float Ceil(float f);
	static inline int CeilToInt(float f);
	static inline float Round(float f);
	static inline int RoundToInt(float f);

	template<typename Type>
	static inline const Type LinearInterp(const Type& a, const Type& b, float t)
	{
		return Type::LinearInterp(a, b, t);
	}

	template<typename Type>
	static inline const Type TriangleInterp(const Type& a, const Type& b, const Type& c, float t0, float t1, float t2)
	{
		return Type::TriangleInterp(a, b, c, t0, t1, t2);
	}

	static inline float LinearInterp(float a, float b, float t)
	{
		return a * (1.f - t) + b * t;
	}

	static inline float TriangleInterp(float a, float b, float c, float t0, float t1, float t2)
	{
		return a * t0 + b * t1 + c * t2;
	}

	static inline float Lerp(float a, float b, float t);
	static inline float BLerp(float a, float b, float c, float d, float t0, float t1);
	static inline float Terp(float a, float b, float c, float x, float y, float z);

	template<class T>
	static inline T Clamp(T value, T min, T max);
	static inline float Clamp01(float value);
	static inline float Repeat(float t, float length);
	static inline float PingPong(float t, float length);

	template<class T>
	static inline T Min(T a, T b);
	template<class T>
	static inline T Min(T a, T b, T c);
	template<class T>
	static inline T MinElement(const T* values, int count);
	template<class T>
	static inline T MinElement(const std::vector<T>& values);
	template<class T>
	static inline T Max(T a, T b);
	template<class T>
	static inline T Max(T a, T b, T c);
	template<class T>
	static inline T MaxElement(const T* values, int count);
	template<class T>
	static inline T MaxElement(const std::vector<T>& values);

	static inline float Sqrt(float value);
	static inline float InvSqrt(float value);
	static inline float Pow(float f, float p);
	static inline float Exp(float f);
	static inline float Log(float f, float d);
    static inline float Log2(float f);
	static inline float Log10(float f);

	static inline bool IsPowerOfTwo(int value);
	static inline int NextPowerOfTwo(int value);

	static inline float Sin(float f);
	static inline float Cos(float f);
	static inline float Tan(float f);
	static inline float Asin(float f);
	static inline float Acos(float f);
	static inline float Atan(float f);
	static inline float Atan2(float y, float x);
};

} // namespace sr


#include "mathf.inl"

#endif //!_MATH_MATHF_H_
