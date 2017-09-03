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
#include <cstdint>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <functional>
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
		/*puts("override new");*/\
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
		/*puts("override delete");*/\
		if (ptr) ::_aligned_free(ptr); \
	}
#else
#define SIMD_ALIGN alignas(16)
#endif
#endif

#define FLIP_RGB(bytes) std::swap(*(bytes + 2), *(bytes + 0));

#include "glfw/include/GLFW/glfw3.h"

typedef uint8_t* rawptr_t;

#endif // !_BASE_HEADER_H_
