#ifndef _BASE_MESH_H_
#define _BASE_MESH_H_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"

namespace rasterizer
{

struct Mesh
{
	Mesh() = default;
	
	static bool LoadMesh(std::vector<Mesh>& meshes, const char* file);

	void BuildNormal();

	std::vector<Vector3> vertices;
	std::vector<u32> indices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
};

}

#endif //!_BASE_MESH_H_
