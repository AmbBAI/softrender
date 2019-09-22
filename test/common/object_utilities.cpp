#include "object_utilities.h"
#include "tinyobjloader/tiny_obj_loader.h"
using namespace sr;

MeshPtr CreatePlane()
{
    //MeshPtr mesh = std::make_shared<Mesh>();
	MeshPtr mesh = MeshPtr(new Mesh());
	mesh->vertices.emplace_back(-1.f, -1.f, 0.f);
	mesh->vertices.emplace_back(1.f, -1.f, 0.f);
	mesh->vertices.emplace_back(1.f, 1.f, 0.f);
	mesh->vertices.emplace_back(-1.f, 1.f, 0.f);

	mesh->normals.emplace_back(0.f, 0.f, -1.f);
	mesh->normals.emplace_back(0.f, 0.f, -1.f);
	mesh->normals.emplace_back(0.f, 0.f, -1.f);
	mesh->normals.emplace_back(0.f, 0.f, -1.f);

	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(0.f, 1.f);

	mesh->colors.emplace_back(1.f, 1.f, 0.f, 0.f);
	mesh->colors.emplace_back(1.f, 0.f, 1.f, 0.f);
	mesh->colors.emplace_back(1.f, 0.f, 0.f, 1.f);
	mesh->colors.emplace_back(1.f, 1.f, 1.f, 1.f);

	mesh->indices = {0, 2, 1, 2, 0, 3};

    mesh->CalculateTangents();

    return mesh;
}

MeshPtr CreateQuad()
{
	MeshPtr mesh = MeshPtr(new Mesh());
	mesh->vertices.emplace_back(-1.f, -1.f, 0.f);
	mesh->vertices.emplace_back(-1.f, 1.f, 0.f);
	mesh->vertices.emplace_back(1.f, -1.f, 0.f);
	mesh->vertices.emplace_back(1.f, 1.f, 0.f);
	mesh->indices = { 0, 1, 2, 2, 1, 3 };

	return mesh;
}

MeshPtr CreateCube()
{
	MeshPtr mesh = MeshPtr(new Mesh());
	mesh->vertices.emplace_back(0.5f, -0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, -0.5f, 0.5f);
	mesh->vertices.emplace_back(0.5f, 0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, 0.5f, 0.5f);
	mesh->vertices.emplace_back(0.5f, 0.5f, -0.5f);
	mesh->vertices.emplace_back(-0.5f, 0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, -0.5f, -0.5f);
	mesh->vertices.emplace_back(-0.5f, -0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, 0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, 0.5f, 0.5f);
	mesh->vertices.emplace_back(0.5f, 0.5f, -0.5f);
	mesh->vertices.emplace_back(-0.5f, 0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, -0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, -0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, -0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, -0.5f, -0.5f);
	mesh->vertices.emplace_back(-0.5f, -0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, 0.5f, 0.5f);
	mesh->vertices.emplace_back(-0.5f, 0.5f, -0.5f);
	mesh->vertices.emplace_back(-0.5f, -0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, -0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, 0.5f, -0.5f);
	mesh->vertices.emplace_back(0.5f, 0.5f, 0.5f);
	mesh->vertices.emplace_back(0.5f, -0.5f, 0.5f);

	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(0.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(0.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(0.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(1.f, 0.f);

	mesh->indices = {
		0, 2, 3, 0, 3, 1,
		8, 4, 5, 8, 5, 9,
		10, 6, 7, 10, 7, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23 };

	mesh->RecalculateNormals();
	mesh->CalculateTangents();
	return mesh;
}

sr::MeshPtr LoadMesh(const char* path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path);

	if (!ret)
	{
		puts(err.c_str());
		return nullptr;
	}

	MeshPtr mesh = std::make_shared<Mesh>();
	typedef std::tuple<uint16_t, uint16_t, uint16_t> MeshIndex;
	std::map<MeshIndex, uint16_t> indexTable;
	for (uint32_t i = 0; i < shapes.size(); ++i)
	{
		for (uint32_t j = 0; j < shapes[i].mesh.indices.size(); ++j)
		{
			const auto& tinyobjIndex = shapes[i].mesh.indices[j];
			MeshIndex meshIndex { (uint16_t)tinyobjIndex.vertex_index, (uint16_t)tinyobjIndex.normal_index, (uint16_t)tinyobjIndex.texcoord_index };
			auto index = indexTable.find(meshIndex);
			if (index == indexTable.end())
			{
				uint16_t realIndex = (uint16_t)indexTable.size();
				int vi = std::get<0>(meshIndex) * 3;
				mesh->vertices.emplace_back(attrib.vertices[vi], attrib.vertices[vi + 1], attrib.vertices[vi + 2]);

				uint16_t ni_ = std::get<1>(meshIndex);
				if (ni_ != (uint16_t)-1)
				{
					int ni = ni_ * 3;
					mesh->normals.emplace_back(attrib.normals[ni], attrib.normals[ni + 1], attrib.normals[ni + 2]);
				}

				uint16_t ti_ = std::get<2>(meshIndex);
				if (ti_ != (uint16_t)-1)
				{
					int ti = ti_ * 2;
					mesh->texcoords.emplace_back(attrib.texcoords[ti], attrib.texcoords[ti + 1]);
				}
				mesh->indices.emplace_back(realIndex);
				indexTable[meshIndex] = realIndex;
			}
			else
			{
				mesh->indices.emplace_back(index->second);
			}
			
		}
	}
	return mesh;
}

