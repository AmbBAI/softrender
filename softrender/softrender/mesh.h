#ifndef _SOFTRENDER_MESH_H_
#define _SOFTRENDER_MESH_H_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/color.h"

namespace sr
{

struct Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;

struct Mesh
{
	Mesh() = default;

	void RecalculateNormals();
	void CalculateTangents();
	int GetVertexCount() { return (int)vertices.size(); }

	enum VertexElement
	{
		VertexElement_Position,
		VertexElement_Normal,
		VertexElement_Tangent,
		VertexElement_Texcoord,
		VertexElement_Color
	};

	std::string name;
	std::vector<Vector3> vertices;
	std::vector<uint16_t> indices;
	std::vector<Color> colors;
	std::vector<Vector3> normals;
	std::vector<Vector4> tangents;
	std::vector<Vector2> texcoords;
};

}

#endif //!_SOFTRENDER_MESH_H_
