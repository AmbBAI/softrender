#include "mesh.h"

#include "thirdpart/tinyobjloader/tiny_obj_loader.h"

namespace rasterizer
{

bool Mesh::LoadMesh(std::vector<Mesh>& meshes, const char* file)
{
	std::vector<tinyobj::shape_t> shape;
	std::vector<tinyobj::material_t> material;
	std::string ret = tinyobj::LoadObj(shape, material, file);

	if (ret.length() != 0)
	{
		printf("%s\n", ret.c_str());
		return false;
	}

	meshes.clear();
	for (int s = 0; s < (int)shape.size(); ++s)
	{
		tinyobj::mesh_t& subMesh = shape[s].mesh;
		std::vector<float>& positions = subMesh.positions;
		std::vector<float>& normals = subMesh.normals;
		std::vector<u32>& indices = subMesh.indices;
		std::vector<float>& texcoords = subMesh.texcoords;

		Mesh mesh;
		for (int i = 0; i + 2 < (int)positions.size(); i += 3)
		{
			mesh.vertices.push_back(Vector3(positions[i], positions[i + 1], positions[i + 2]));
		}
		mesh.indices.assign(indices.begin(), indices.end());
		for (int i = 0; i + 2 < (int)normals.size(); i += 3)
		{
			mesh.normals.push_back(Vector3(normals[i], normals[i + 1], normals[i + 2]));
		}
		for (int i = 0; i + 1 < (int)texcoords.size(); i += 2)
		{
			mesh.texcoords.push_back(Vector2(texcoords[i], texcoords[i + 1]));
		}
		meshes.push_back(mesh);
	}
	return true;
}

void Mesh::BuildNormal()
{
	normals.assign(vertices.size(), Vector3::zero);
	//std::vector < u32 > faceCount(vertices.size(), 0);
	for (int i = 0; i + 2 < (int)indices.size(); i += 3)
	{
		int v0 = indices[i];
		int v1 = indices[i + 1];
		int v2 = indices[i + 2];

		Vector3 edge1 = vertices[v1].Subtract(vertices[v0]);
		Vector3 edge2 = vertices[v2].Subtract(vertices[v0]);

		Vector3 normal = edge1.Cross(edge2);
		normals[v0] = normals[v0].Add(normal);
		normals[v1] = normals[v1].Add(normal);
		normals[v2] = normals[v2].Add(normal);

		//faceCount[v0] += 1;
		//faceCount[v1] += 1;
		//faceCount[v2] += 1;
	}

	for (int i = 0; i < (int)normals.size(); ++i)
	{
		//if (faceCount[i] > 0)
		//{
			normals[i] = normals[i].Normalize();
		//}
	}
}

}