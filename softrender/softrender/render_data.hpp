#ifndef _SOFTRENDER_RENDER_DATA_HPP_
#define _SOFTRENDER_RENDER_DATA_HPP_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "softrender/srtypes.hpp"
#include "softrender/buffer.h"
#include "softrender/mesh.h"

namespace sr
{

class RenderData
{
public:
	template<typename VertexType>
	bool AssetVerticesIndicesBuffer(const Mesh& mesh)
	{
		uint32_t vertexTypeSize = sizeof(VertexType);
		auto& elements = VertexType::elements();
		uint32_t vertexElementsSize = 0;
		for (auto element : elements)
		{
			switch (element)
			{
			case Mesh::VertexElement_Position:
				vertexElementsSize += sizeof(Vector3);
				break;
			case Mesh::VertexElement_Normal:
				vertexElementsSize += sizeof(Vector3);
				assert (mesh.normals.size() == mesh.vertices.size());
				break;
			case Mesh::VertexElement_Tangent:
				vertexElementsSize += sizeof(Vector4);
				assert(mesh.tangents.size() == mesh.vertices.size());
				break;
			case Mesh::VertexElement_Color:
				vertexElementsSize += sizeof(Color);
				assert(mesh.colors.size() == mesh.vertices.size());
				break;
			case Mesh::VertexElement_Texcoord:
				vertexElementsSize += sizeof(Vector2);
				assert(mesh.texcoords.size() == mesh.vertices.size());
				break;
			default:
				break;
			}
		}
		assert(vertexElementsSize == vertexTypeSize);
		if (vertexElementsSize != vertexTypeSize)
		{
			return false;
		}

		std::vector<VertexType> vertexBuffer;
		for (uint32_t i = 0; i < mesh.vertices.size(); ++i)
		{
			VertexType v;
			rawptr_t p = (rawptr_t)&v;
			for (Mesh::VertexElement element : elements)
			{
				switch (element)
				{
				case Mesh::VertexElement_Position:
					*(Vector3*)p = mesh.vertices[i];
					p += sizeof(Vector3);
					break;
				case Mesh::VertexElement_Normal:
					*(Vector3*)p = mesh.normals[i];
					p += sizeof(Vector3);
					break;
				case Mesh::VertexElement_Tangent:
					*(Vector4*)p = mesh.tangents[i];
					p += sizeof(Vector4);
					break;
				case Mesh::VertexElement_Color:
					*(Color*)p = mesh.colors[i];
					p += sizeof(Color);
					break;
				case Mesh::VertexElement_Texcoord:
					*(Vector2*)p = mesh.texcoords[i];
					p += sizeof(Vector2);
					break;
				default:
					break;
				}
			}
			vertexBuffer.push_back(v);
		}
		if (!AssignVertexBuffer(vertexBuffer))
		{
			return false;
		}
		AssignIndexBuffer(mesh.indices);
		return true;
	}

	template<typename VertexType>
	bool AssignVertexBuffer(const std::vector<VertexType>& vertices)
	{
		static const int VERTEX_MAX_COUNT = (1 << 16) - 1;
		int count = (int)vertices.size();
		assert(count <= VERTEX_MAX_COUNT);
		if (count > VERTEX_MAX_COUNT) return false;
		vertexBuffer.Assign(vertices);
		vertexCount = count;
		vertexSize = sizeof(VertexType);
		return true;
	}

	int GetVertexCount()
	{
		return vertexCount;
	}

	template<typename VertexType = void>
	VertexType* GetVertexData(int index)
	{
		return (VertexType*)vertexBuffer[index];
	}

	bool AssignIndexBuffer(const std::vector<uint16_t>& indices)
	{
		int count = (int)indices.size();
		indexBuffer.Assign(indices);
		indexCount = count;
		return true;
	}

	int GetIndexCount()
	{
		return indexCount;
	}

	int GetPrimitiveCount()
	{
		return indexCount / 3;
	}

	bool GetTrianglePrimitive(int id, Triangle<uint16_t>& triangle)
	{
		int offset = id * 3;
		if (offset + 3 > indexCount) return false;
		auto itor = indexBuffer.itor;
		itor.Seek(offset);
		triangle.v0 = *(uint16_t*)itor.Get();
		triangle.v1 = *(uint16_t*)itor.Get();
		triangle.v2 = *(uint16_t*)itor.Get();
		return true;
	}

private:
	Buffer vertexBuffer;
	int vertexCount = 0;
	int vertexSize = 0;

	Buffer indexBuffer;
	int indexCount = 0;
};

} // namespace sr

#endif // !_SOFTRENDER_RENDER_DATA_HPP_
