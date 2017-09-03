#ifndef _OBJECT_UTILITIES_H_
#define _OBJECT_UTILITIES_H_

#include "softrender.h"

sr::MeshPtr CreateQuad();
sr::MeshPtr CreatePlane();
sr::MeshPtr CreateCube();

sr::MeshPtr LoadMesh(const char* path);

#endif // !_OBJECT_UTILITIES_H_
