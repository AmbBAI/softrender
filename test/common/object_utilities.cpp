#include "object_utilities.h"
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

