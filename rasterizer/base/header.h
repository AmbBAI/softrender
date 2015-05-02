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
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

#if _USE_OPENMP_
#include "omp.h"
#endif

#if _MATH_SIMD_INTRINSIC_
#include "smmintrin.h"
#if defined(_MSC_VER)
#define SIMD_ALIGN __declspec(align(16))
#define MEMALIGN_NEW_OPERATOR_OVERRIDE(align) \
	void* operator new(size_t size) { \
		if (size == 0) size = 1; \
		void* p = nullptr; \
		while ((p = _aligned_malloc(size, align)) == 0) { \
			std::new_handler nh = std::get_new_handler(); \
			if (nh) nh(); \
			else throw std::bad_alloc(); \
		} \
		return p; \
	}
#define MEMALIGN_DELETE_OPERATOR_OVERRIDE \
	void operator delete(void* ptr) { \
		if (ptr) ::_aligned_free(ptr); \
	}
#else
#define SIMD_ALIGN alignas(16)
#endif
#endif

#ifdef _USE_GLEW_
#include "glew/include/GL/glew.h"
#define NANOVG_GLEW
#endif
#include "glfw/include/GLFW/glfw3.h"

typedef unsigned char u8;
typedef unsigned int u32;

#endif // !_BASE_HEADER_H_
