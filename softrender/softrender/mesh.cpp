#include "mesh.h"


namespace sr
{

bool Mesh::LoadMesh(std::vector<MeshPtr>& meshes, const char* file)
{
	std::vector<tinyobj::shape_t> osjShapes;
	std::vector<tinyobj::material_t> objMaterials;
	std::string filePath(file);
	std::replace(filePath.begin(), filePath.end(), '\\', '/');
    std::string::size_type pos = filePath.rfind('/');
	std::string fileDir = "";
	if (pos != filePath.npos) fileDir = filePath.substr(0, pos + 1);
	std::string ret = tinyobj::LoadObj(osjShapes, objMaterials, filePath.c_str(), fileDir.c_str());

	if (ret.length() != 0)
	{
		printf("%s\n", ret.c_str());
		return false;
	}

	std::vector<MaterialPtr> materials;
    Material::LoadMaterial(materials, objMaterials, fileDir.c_str());
    Mesh::LoadMesh(meshes, osjShapes, materials);
    return true;
}

void Mesh::LoadMesh(std::vector<MeshPtr>& meshes, const std::vector<tinyobj::shape_t>& objMeshes, const std::vector<MaterialPtr>& materials)
{
    meshes.clear();
    for (auto& s : objMeshes)
    {
        //MeshPtr mesh = std::make_shared<Mesh>();
		MeshPtr mesh = MeshPtr(new Mesh());
		mesh->name = s.name;
		auto& m = s.mesh;

		auto& _p = m.positions;
		auto& _n = m.normals;
		auto& _tc = m.texcoords;
		int vc = _p.size() / 3;
		for (int i = 0; i < vc; ++i) mesh->vertices.emplace_back(_p[i * 3], _p[i * 3 + 1], _p[i * 3 + 2]);
		for (int i = 0; i < vc; ++i) mesh->normals.emplace_back(_n[i * 3], _n[i * 3 + 1], _n[i * 3 + 2]);
		for (int i = 0; i < vc; ++i) mesh->texcoords.emplace_back(_tc[i * 2], _tc[i * 2 + 1]);

		for (int i = 0; i < (int)m.indices.size(); ++i)
		{
			assert(_i[i] < UINT16_MAX);
			mesh->indices.emplace_back((uint16_t)m.indices[i]);
		}

        if (mesh->normals.size() == mesh->vertices.size()
            && mesh->texcoords.size() == mesh->vertices.size())
        {
            mesh->CalculateTangents();
        }

		int matID = -1;
		int startIndex = 0;
		int primitiveCount = 0;
		for (int i = 0; i < (int)s.mesh.material_ids.size(); ++i)
        {
			int id = s.mesh.material_ids[i];
			//assert(id != -1);
			if (matID != id)
			{
				if (matID != -1) 
				{
					mesh->materials.emplace_back(materials[matID], startIndex, primitiveCount);
				}
				matID = id;
				startIndex = i;
				primitiveCount = 1;
			}
			else
			{
				primitiveCount += 1;
			}
        }
		if (matID != -1)
		{
			mesh->materials.emplace_back(materials[matID], startIndex, primitiveCount);
		}
        
        meshes.push_back(mesh);
    }
}
    
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