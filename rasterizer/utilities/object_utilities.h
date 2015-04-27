#ifndef _RASTERIZER_OBJECT_UTILITIES_H_
#define _RASTERIZER_OBJECT_UTILITIES_H_

#include "rasterizer.h"


rasterizer::MeshPtr CreatePlane();

void LoadSponzaMesh(std::vector<rasterizer::MeshPtr>& mesh, rasterizer::Transform& trans);


#endif // !_RASTERIZER_OBJECT_UTILITIES_H_
