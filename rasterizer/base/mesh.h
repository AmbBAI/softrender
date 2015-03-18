#ifndef _BASE_MESH_H_
#define _BASE_MESH_H_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "base/material.h"

namespace rasterizer
{

struct Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;

struct Mesh
{
	Mesh() = default;
	
	static bool LoadMesh(std::vector<MeshPtr>& meshes, const char* file);

	void RecalculateNormals();
	void CalculateTangents();

	std::vector<Vector3> vertices;
	std::vector<u32> indices;
	std::vector<Vector3> normals;
	std::vector<Vector3> tangents;
	std::vector<Vector2> texcoords;
	std::vector<MaterialPtr> materials;
};

}

#endif //!_BASE_MESH_H_
