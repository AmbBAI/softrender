#include "mesh.h"

#include "tinyobjloader/tiny_obj_loader.h"

namespace rasterizer
{

bool Mesh::LoadMesh(std::vector<MeshPtr>& meshes, const char* file)
{
	std::vector<tinyobj::shape_t> shape;
	std::vector<tinyobj::material_t> material;
	std::string filePath(file);
	std::replace(filePath.begin(), filePath.end(), '\\', '/');
    std::string::size_type pos = filePath.rfind('/');
	std::string fileDir = "";
	if (pos != filePath.npos) fileDir = filePath.substr(0, pos + 1);
	std::string ret = tinyobj::LoadObj(shape, material, filePath.c_str(), fileDir.c_str());

	if (ret.length() != 0)
	{
		printf("%s\n", ret.c_str());
		return false;
	}

	std::vector<MaterialPtr> materials;
    for (auto m : material)
	{
		MaterialPtr newM(new Material());
		newM->ambient = Color(1.f, m.ambient[0], m.ambient[1], m.ambient[2]);
		newM->diffuse = Color(1.f, m.diffuse[0], m.diffuse[1], m.diffuse[2]);
		newM->specular = Color(1.f, m.ambient[0], m.ambient[1], m.ambient[2]);
		newM->shininess = m.shininess;
		newM->emission = Color(1.f, m.emission[0], m.emission[1], m.emission[2]);
		if (m.diffuse_texname.size() > 0)
		{
			std::string texPath = fileDir + m.diffuse_texname;
            std::replace(texPath.begin(), texPath.end(), '\\', '/');
			newM->diffuseTexture = Texture::LoadTexture(texPath.c_str());
            newM->diffuseTexture->GenerateMipmaps();
		}
		if (m.normal_texname.size() > 0)
		{
			std::string texPath = fileDir + m.normal_texname;
			std::replace(texPath.begin(), texPath.end(), '\\', '/');
			newM->normalTexture = Texture::LoadTexture(texPath.c_str());
		}
		else
		{ // check bump
			auto param_itor = m.unknown_parameter.find("map_bump");
			if (param_itor != m.unknown_parameter.end())
			{
				//TODO convert bump to normal
			}
		}

		materials.push_back(newM);
	}

	meshes.clear();
    for (auto s : shape)
	{
		std::vector<float>& positions = s.mesh.positions;
		std::vector<float>& normals = s.mesh.normals;
		std::vector<u32>& indices = s.mesh.indices;
		std::vector<float>& texcoords = s.mesh.texcoords;

		MeshPtr mesh(new Mesh());
		for (int i = 0; i + 2 < (int)positions.size(); i += 3)
		{
			mesh->vertices.push_back(Vector3(positions[i], positions[i + 1], positions[i + 2]));
		}
		mesh->indices.assign(indices.begin(), indices.end());
		for (int i = 0; i + 2 < (int)normals.size(); i += 3)
		{
			mesh->normals.push_back(Vector3(normals[i], normals[i + 1], normals[i + 2]));
		}
		for (int i = 0; i + 1 < (int)texcoords.size(); i += 2)
		{
			mesh->texcoords.push_back(Vector2(texcoords[i], texcoords[i + 1]));
		}

		if (mesh->normals.size() == mesh->vertices.size()
			&& mesh->texcoords.size() == mesh->vertices.size())
		{
			mesh->CalculateTangents();
		}

        for (auto id : s.mesh.material_ids)
		{
			if (id < 0 || id >= (int) materials.size()) continue;
			mesh->materials.push_back(materials[id]);
		}

		meshes.push_back(mesh);
	}
	return true;
}

void Mesh::RecalculateNormals()
{
    normals.clear();
    
	u32 vertexCount = vertices.size();
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

	CalculateTangents();
}

void Mesh::CalculateTangents()
{
	tangents.clear();

	u32 vertexCount = normals.size();
	tangents.assign(vertexCount, Vector3::zero);
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
		tangents[i] = (tan1[i] - normals[i] * normals[i].Dot(tan1[i])).Normalize();
		float w = normals[i].Cross(tan1[i]).Dot(tan2[i]) < 0.f ? -1.f : 1.f;
		Vector3 binormal = normals[i].Cross(tangents[i]).Multiply(w);
		tangents[i] = binormal.Cross(normals[i]);
	}
}

}