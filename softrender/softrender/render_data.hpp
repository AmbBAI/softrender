#ifndef _SOFTRENDER_RENDER_DATA_HPP_
#define _SOFTRENDER_RENDER_DATA_HPP_

#include "base/header.h"
#include "softrender/vertex.hpp"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "softrender/buffer.h"

namespace sr
{

class RenderData
{
public:
	enum PrimitiveType
	{
		PrimitiveType_Point,
		PrimitiveType_Line,
		PrimitiveType_LineStrip,
		PrimitiveType_Triangle,
		PrimitiveType_TriangleStrip,
	};

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

	bool AssignIndexBuffer(const std::vector<uint16_t>& indices, PrimitiveType type = PrimitiveType_Triangle)
	{
		int count = (int)indices.size();
		indexBuffer.Assign(indices);
		indexCount = count;
		primitiveType = type;
		return true;
	}

	int GetIndexCount()
	{
		return indexCount;
	}

	int GetPrimitiveCount()
	{
		switch (primitiveType)
		{
		case PrimitiveType_Point:
			return indexCount;
		case PrimitiveType_Line:
			return indexCount / 2;
		case PrimitiveType_LineStrip:
			return indexCount - 1;
		case PrimitiveType_Triangle:
			return indexCount / 3;
		case PrimitiveType_TriangleStrip:
			return indexCount - 2;
		default:
			return 0;
		}
	}

	// TODO GetPointPrimitive(int id, u16& point);
	// TODO GetLinePrimitive(int id, Line<u16>& line);

	bool GetTrianglePrimitive(int id, Triangle<uint16_t>& triangle)
	{
		switch (primitiveType)
		{
		case PrimitiveType_Triangle:
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
		case PrimitiveType_TriangleStrip:
			// TODO
			return true;
		}
		return false;
	}

private:
	PrimitiveType primitiveType = PrimitiveType_Triangle;

	Buffer vertexBuffer;
	int vertexCount = 0;
	int vertexSize = 0;

	Buffer indexBuffer;
	int indexCount = 0;
};

} // namespace rasterizer

#endif // !_SOFTRENDER_RENDER_DATA_HPP_
