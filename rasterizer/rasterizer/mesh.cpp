#include "mesh.h"


namespace rasterizer
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
        auto& _p = s.mesh.positions;
        auto& _n = s.mesh.normals;
        auto& _i = s.mesh.indices;
        auto& _tc = s.mesh.texcoords;
        
        MeshPtr mesh(new Mesh());
        for (int i = 0; i + 2 < (int)_p.size(); i += 3)
        {
            mesh->vertices.push_back(Vector3(_p[i], _p[i + 1], _p[i + 2]));
        }
        mesh->indices.assign(_i.begin(), _i.end());
        for (int i = 0; i + 2 < (int)_n.size(); i += 3)
        {
            mesh->normals.push_back(Vector3(_n[i], _n[i + 1], _n[i + 2]));
        }
        for (int i = 0; i + 1 < (int)_tc.size(); i += 2)
        {
            mesh->texcoords.push_back(Vector2(_tc[i], _tc[i + 1]));
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

    if (texcoords.size() == vertexCount) CalculateTangents();
}

void Mesh::CalculateTangents()
{
	u32 vertexCount = normals.size();
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