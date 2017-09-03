#include "mesh.h"


namespace sr
{

void Mesh::RecalculateNormals()
{
    normals.clear();
    
	uint32_t vertexCount = vertices.size();
	normals.assign(vertexCount, Vector3::zero);
	for (int i = 0; i + 2 < (int)indices.size(); i += 3)
	{
		int v0 = indices[i];
		int v1 = indices[i + 1];
		int v2 = indices[i + 2];

		Vector3 edge1 = vertices[v1] - vertices[v0];
		Vector3 edge2 = vertices[v2] - vertices[v0];

		Vector3 normal = edge1.Cross(edge2);
		normals[v0] += normal;
		normals[v1] += normal;
		normals[v2] += normal;
	}

	for (int i = 0; i < (int)vertexCount; ++i)
	{
		normals[i] = normals[i].Normalize();
	}

    if (texcoords.size() == vertexCount) CalculateTangents();
}

void Mesh::CalculateTangents()
{
	uint32_t vertexCount = normals.size();
	tangents.resize(vertexCount);
	std::vector<Vector3> tan1(vertexCount, Vector3::zero);
	std::vector<Vector3> tan2(vertexCount, Vector3::zero);
	for (int i = 0; i + 2 < (int)indices.size(); i += 3)
	{
		int v0 = indices[i];
		int v1 = indices[i + 1];
		int v2 = indices[i + 2];

		Vector3 edge1 = vertices[v1] - vertices[v0];
		Vector3 edge2 = vertices[v2] - vertices[v0];

		Vector2 st1 = texcoords[v1] - texcoords[v0];
		Vector2 st2 = texcoords[v2] - texcoords[v0];

		float r = 1.f / (st1.x * st2.y - st2.x * st1.y);
		Vector3 sdir = Vector3(
			(st2.y * edge1.x - st1.y * edge2.x) * r,
			(st2.y * edge1.y - st1.y * edge2.y) * r,
			(st2.y * edge1.z - st1.y * edge2.z) * r);

		Vector3 tdir = Vector3(
			(st1.x * edge2.x - st2.x * edge1.x) * r,
			(st1.x * edge2.y - st2.x * edge1.y) * r,
			(st1.x * edge2.z - st2.x * edge1.z) * r);

		tan1[v0] += sdir;
		tan1[v1] += sdir;
		tan1[v2] += sdir;

		tan2[v0] += tdir;
		tan2[v1] += tdir;
		tan2[v2] += tdir;
	}

	for (int i = 0; i < (int)vertexCount; ++i)
	{
		tangents[i].xyz = (tan1[i] - normals[i] * (normals[i].Dot(tan1[i]))).Normalize();
		tangents[i].w = normals[i].Cross(tan1[i]).Dot(tan2[i]) < 0.f ? -1.f : 1.f;
	}
}

}