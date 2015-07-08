#ifndef _BASE_MESH_H_
#define _BASE_MESH_H_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "rasterizer/material.h"

#include "tinyobjloader/tiny_obj_loader.h"

namespace rasterizer
{

struct Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;

struct Mesh
{
	Mesh() = default;
	
	static bool LoadMesh(std::vector<MeshPtr>& meshes, const char* file);
    static void LoadMesh(std::vector<MeshPtr>& meshes, const std::vector<tinyobj::shape_t>& shapes, const std::vector<MaterialPtr>& materials);

	void RecalculateNormals();
	void CalculateTangents();
	int GetVertexCount() { return (int)vertices.size(); }

	std::string name;
	std::vector<Vector3> vertices;
	std::vector<uint16_t> indices;
	std::vector<Color> colors;
	std::vector<Vector3> normals;
	std::vector<Vector4> tangents;
	std::vector<Vector2> texcoords;
	std::vector<std::tuple<MaterialPtr, int, int> > materials;
};

}

#endif //!_BASE_MESH_H_
