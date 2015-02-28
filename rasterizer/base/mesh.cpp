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
		meshes.push_back(mesh);
	}
	return true;
}

}