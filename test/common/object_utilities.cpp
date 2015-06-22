#include "object_utilities.h"
using namespace rasterizer;

MeshPtr CreatePlane(const MaterialPtr& mat)
{
    //MeshPtr mesh = std::make_shared<Mesh>();
	MeshPtr mesh = MeshPtr(new Mesh());
	mesh->vertices.emplace_back(-1.f, -1.f, 0.f);
	mesh->vertices.emplace_back(1.f, -1.f, 0.f);
	mesh->vertices.emplace_back(1.f, 1.f, 0.f);
	mesh->vertices.emplace_back(-1.f, 1.f, 0.f);
    
	mesh->normals.emplace_back(0.f, 0.f, 1.f);
	mesh->normals.emplace_back(0.f, 0.f, 1.f);
	mesh->normals.emplace_back(0.f, 0.f, 1.f);
	mesh->normals.emplace_back(0.f, 0.f, 1.f);
    
	mesh->texcoords.emplace_back(0.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 0.f);
	mesh->texcoords.emplace_back(1.f, 1.f);
	mesh->texcoords.emplace_back(0.f, 1.f);

	mesh->materials.emplace_back(mat, 0, 4);

	mesh->indices = {0, 1, 2, 2, 3, 0};

    mesh->CalculateTangents();
    
    return mesh;
}

