#ifndef _BASE_HEADER_H_
#define _BASE_HEADER_H_

#include <cstdio>
#include <ctime>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cfloat>
#include <cassert>

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#if _MATH_SIMD_INTRINSIC_
#include "smmintrin.h"
#if defined(_MSC_VER)
#define SIMD_ALIGN __declspec(align(16))
#else
#define SIMD_ALIGN alignas(16)
#endif
#endif

#include "glfw/include/GLFW/glfw3.h"

typedef unsigned char u8;
typedef unsigned int u32;

#endif // !_BASE_HEADER_H_
