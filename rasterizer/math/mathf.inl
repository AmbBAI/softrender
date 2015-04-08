#ifndef _MATH_INLINE_
#define _MATH_INLINE_

namespace rasterizer
{

float Mathf::Abs(float f)
{
	return ::fabs(f);
}

int Mathf::Abs(int i)
{
	if (i < 0) return -i;
	return i;
}

float Mathf::Sign(float f)
{
	return f < 0.f ? -1.f : 1.f;
}
bool Mathf::Approximately(float a, float b)
{
	return Mathf::Abs(a - b) < Mathf::epsilon;
}

float Mathf::Trunc(float f)
{
	return (float)Mathf::TruncToInt(f);
}
int Mathf::TruncToInt(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return _mm_cvtt_ss2si(_mm_set_ss(f));
#else
	return (int)f;
#endif
}

float Mathf::Fractional(float f)
{
	return f - Mathf::Trunc(f);
}

float Mathf::Floor(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return (float)Mathf::FloorToInt(f);
#else
	return ::floor(f);
#endif
}
int Mathf::FloorToInt(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return _mm_cvt_ss2si(_mm_set_ss(f + f - 0.5f)) >> 1;
#else
	return (int)(Mathf::Floor(f) + Mathf::epsilon);
#endif
}
float Mathf::Ceil(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return (float)Mathf::CeilToInt(f);
#else
	return ::ceil(f);
#endif
}
int Mathf::CeilToInt(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return -(_mm_cvt_ss2si(_mm_set_ss(-0.5f - (f + f))) >> 1);
#else
	return (int)(Mathf::Ceil(f) + Mathf::epsilon);
#endif
}
float Mathf::Round(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return (float)Mathf::RoundToInt(f);
#else
	return ::round(f);
#endif
}
int Mathf::RoundToInt(float f)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return _mm_cvt_ss2si( _mm_set_ss(f + f + 0.5f) ) >> 1;
#else
	return (int)(Mathf::Round(f) + Mathf::epsilon);
#endif
}

float Mathf::Clamp01(float value)
{
	return Mathf::Clamp(value, 0.f, 1.f);
}

float Mathf::Repeat(float t, float length)
{
	assert(length != 0.f);

	bool negative = false;
	if (length < 0.f)
	{
		negative = true;
		length = -length;
		t = -t;
	}
	if (0 <= t && t < length) return negative ? -t : t;

	float ret = t - Mathf::Floor(t / length) * length;
	return negative ? -ret : ret;
}

float Mathf::PingPong(float t, float length)
{
	assert(length != 0.f);

	bool negative = false;
	if (length < 0.f)
	{
		negative = true;
		length = -length;
		t = -t;
	}
	if (0 <= t && t < length) return negative ? -t : t;

	float fTDivL = t / length;
	int ifTDivL = Mathf::FloorToInt(fTDivL);
	float ret = t - Mathf::Floor(fTDivL) * length;
	if (ifTDivL & 1) return negative ? -ret : ret;
	else return negative ? ret - length : length - ret;
}

template<class T>
T Mathf::Min(T a, T b)
{
	return std::min(a, b);
}

template<class T>
T Mathf::Min(T a, T b, T c)
{
	return Mathf::Min(a, Mathf::Min(b, c));
}

template<class T>
T Mathf::MinElement(const T* values, int count)
{
	assert(values != nullptr);
	assert(count > 0);

	return *std::min_element(values, values + count);
}

template<class T>
T Mathf::MinElement(const std::vector<T>& values)
{
	return *std::min_element(values.begin(), values.end());
}

template<class T>
T Mathf::Max(T a, T b)
{
	return std::max(a, b);
}

template<class T>
T Mathf::Max(T a, T b, T c)
{
	return Mathf::Max(a, Mathf::Max(b, c));
}

template<class T>
T Mathf::MaxElement(const T* values, int count)
{
	assert(values != nullptr);
	assert(count > 0);

	return *std::max_element(values, values + count);
}

template<class T>
T Mathf::MaxElement(const std::vector<T>& values)
{
	return *std::max_element(values.begin(), values.end());
}

template<class T>
T Mathf::Clamp(T value, T min, T max)
{
	return value > max ? max : Mathf::Max(value, min);
}

float Mathf::Sin(float f)
{
	return ::sin(f);
}
float Mathf::Cos(float f)
{
	return ::cos(f);
}
float Mathf::Tan(float f)
{
	return ::tan(f);
}
float Mathf::Asin(float f)
{
	return ::asin(f);
}
float Mathf::Acos(float f)
{
	return ::acos(f);
}
float Mathf::Atan(float f)
{
	return ::atan(f);
}
float Mathf::Atan2(float y, float x)
{
	return ::atan2(y, x);
}

float Mathf::Sqrt(float value)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(value)));
#else
	return ::sqrt(value);
#endif
}

float Mathf::InvSqrt(float value)
{
#ifdef _MATH_SIMD_INTRINSIC_
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(value)));
#else
	return 1.f / Sqrt(value);
#endif
}

float Mathf::Pow(float f, float p)
{
	return ::pow(f, p);
}

float Mathf::Exp(float f)
{
	return ::exp(f);
}

float Mathf::Log(float f, float d)
{
    return ::log(f) / ::log(d);
}

float Mathf::Log10(float f)
{
	return ::log10(f);
}

bool Mathf::IsPowerOfTwo(int value)
{
	if (value <= 0) return false;
	return (value & (value - 1)) == 0;
}

int Mathf::NextPowerOfTwo(int value)
{
	if (value <= 0) return 1;
	if (Mathf::IsPowerOfTwo(value)) return value << 1;

	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	++value;
	return value;
}

float Mathf::Lerp(float a, float b, float t)
{
	return a * (1.f - t) + b * t;
}

} // namespace rasterizer

#endif // !_MATH_INLINE_
