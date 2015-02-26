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

#include <vector>
#include <algorithm>

#ifdef _WINDOWS
#include <Windows.h>
#include <omp.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "thirdpart/glut/glut.h"
#elif _LINUX
#include <omp.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif _MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

typedef unsigned char u8;
typedef unsigned int u32;

#endif // !_BASE_HEADER_H_
