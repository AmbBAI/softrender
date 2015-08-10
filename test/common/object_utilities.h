#ifndef _OBJECT_UTILITIES_H_
#define _OBJECT_UTILITIES_H_

#include "softrender.h"

template<typename Type>
struct MeshWrapper
{
	std::string name;
	std::vector<Type> vertices;
	std::vector<uint16_t> indices;
	std::vector<std::tuple<sr::MaterialPtr, int, int> > materials;
};

sr::MeshPtr CreatePlane();

#endif // !_OBJECT_UTILITIES_H_
