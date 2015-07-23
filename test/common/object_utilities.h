#ifndef _OBJECT_UTILITIES_H_
#define _OBJECT_UTILITIES_H_

#include "rasterizer.h"

template<typename Type>
struct MeshWrapper
{
	std::string name;
	std::vector<Type> vertices;
	std::vector<uint16_t> indices;
	std::vector<std::tuple<rasterizer::MaterialPtr, int, int> > materials;
};

rasterizer::MeshPtr CreatePlane();

#endif // !_OBJECT_UTILITIES_H_
