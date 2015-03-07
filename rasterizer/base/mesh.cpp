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

void Mesh::RecalculateNormals()
{
	u32 vertexCount = vertices.size();
	normals.assign(vertexCount, Vector3::zero);
	tangents.assign(vertexCount, Vector4());
	std::vector<Vector3> tan1(vertexCount, Vector3::zero);
	std::vector<Vector3> tan2(vertexCount, Vector3::zero);
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

		Vector2 st1 = texcoords[v1].Subtract(texcoords[v0]);
		Vector2 st2 = texcoords[v2].Subtract(texcoords[v0]);

		float r = 1.f / (st1.x * st2.y - st2.x * st1.y);
		Vector3 dir1 = Vector3(
			(st2.y * edge1.x - st1.y * edge2.x) * r,
			(st2.y * edge1.y - st1.y * edge2.y) * r,
			(st2.y * edge1.z - st1.y * edge2.z) * r);

		Vector3 dir2 = Vector3(
			(st1.x * edge2.x - st2.x * edge1.x) * r,
			(st1.x * edge2.y - st2.x * edge1.y) * r,
			(st1.x * edge2.z - st2.x * edge1.z) * r);

		tan1[v0] = tan1[v0] + dir1;
		tan1[v1] = tan1[v1] + dir1;
		tan1[v2] = tan1[v2] + dir1;

		tan2[v0] = tan2[v0] + dir2;
		tan2[v1] = tan2[v1] + dir2;
		tan2[v2] = tan2[v2] + dir2;
	}

	for (int i = 0; i < (int)vertexCount; ++i)
	{
		normals[i] = normals[i].Normalize();
		tangents[i] = (tan1[i] - normals[i] * normals[i].Dot(tan1[i])).Normalize();
		tangents[i].w = normals[i].Cross(tan1[i]).Dot(tan2[i]) < 0.f ? -1.f : 1.f ;
	}
}

}