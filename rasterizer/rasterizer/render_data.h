#ifndef _RASTERIZER_RENDER_DATA_H_
#define _RASTERIZER_RENDER_DATA_H_

#include "base/header.h"
#include "rasterizer/vertex.hpp"
#include "rasterizer/light.hpp"
#include "rasterizer/material.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "rasterizer/buffer.h"

namespace rasterizer
{

class RenderData;
class VaryingDataBuffer;
struct VertexVaryingData
{
	VaryingDataBuffer* varyingDataBuffer = nullptr;

	void* data = nullptr;

	Vector4 position;
	u32 clipCode = 0x00;

	VertexVaryingData() = default;
	VertexVaryingData(VaryingDataBuffer* _varyingDataBuffer) : varyingDataBuffer(_varyingDataBuffer) {}
	static VertexVaryingData LinearInterp(const VertexVaryingData& a, const VertexVaryingData& b, float t);
};

struct PixelVaryingData
{
	VaryingDataBuffer* varyingDataBuffer = nullptr;
	void* data = nullptr;

	static PixelVaryingData TriangleInterp(const VertexVaryingData& v0, const VertexVaryingData& v1, const VertexVaryingData& v2, float x, float y, float z);
};

enum VaryingDeclUsage
{
	VaryingDataDeclUsage_POSITION,
	VaryingDataDeclUsage_NORMAL,
	VaryingDataDeclUsage_COLOR,
	VaryingDataDeclUsage_TEXCOORD,
};

enum VaryingDeclFormat
{
	VaryingDataDeclFormat_Float,
	VaryingDataDeclFormat_Vector2,
	VaryingDataDeclFormat_Vector3,
	VaryingDataDeclFormat_Vector4,
};

struct VaryingDataLayout
{
	int offset;
	VaryingDeclUsage usage;
	VaryingDeclFormat format;
};

struct VaryingDataDecl
{
	std::vector<VaryingDataLayout> layout;
	int size;
	int positionOffset = -1;
	int texCoordOffset = -1;

	static int GetFormatSize(VaryingDeclFormat format)
	{
		switch (format)
		{
		case VaryingDataDeclFormat_Float:
			return 4;
		case VaryingDataDeclFormat_Vector2:
			return 8;
		case VaryingDataDeclFormat_Vector3:
			return 12;
		case VaryingDataDeclFormat_Vector4:
			return 16;
		default:
			return 0;
		}
	}

	void LinearInterp(void* output, const void* a, const void* b, float t) const;
	void TriangleInterp(void* output, const void* a, const void* b, const void* c, float x, float y, float z) const;
};

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

	static const u32 VERTEX_MAX_COUNT;

	template<typename VertexType>
	bool AssignVertexBuffer(const std::vector<VertexType>& vertices)
	{
		int count = (int)vertices.size();
		assert(count <= (int)VERTEX_MAX_COUNT);
		if (count > (int)VERTEX_MAX_COUNT) return false;
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

	bool AssignIndexBuffer(const std::vector<u16>& indices, PrimitiveType type = PrimitiveType_Triangle)
	{
		int count = (int)indices.size();
		indexBuffer.Assign(indices);
		indexCount = count;
		primitiveType = type;
		return true;
	}

	int GetIndicesCount()
	{
		return indexCount;
	}

	int GetPrimitiveCount()
	{
		/* TODO
		switch (primitiveType)
		{
		case PrimitiveType_Point:
			break;
		case PrimitiveType_Line:
			break;
		case PrimitiveType_LineStrip:
			break;
		case PrimitiveType_Triangle:
			break;
		case PrimitiveType_TriangleStrip:
			break;
		}
		*/

		return indexCount / 3;
	}

	// TODO GetPointPrimitive(int id, u16& point);
	// TODO GetLinePrimitive(int id, Line<u16>& line);

	bool GetTrianglePrimitive(int id, Triangle<u16>& triangle)
	{
		switch (primitiveType)
		{
		case PrimitiveType_Triangle:
		{
		   int offset = id * 3;
		   if (offset + 3 > indexCount) return false;
		   auto itor = indexBuffer.itor;
		   itor.Seek(offset);
		   triangle.v0 = *(u16*)itor.Get();
		   triangle.v1 = *(u16*)itor.Next();
		   triangle.v2 = *(u16*)itor.Next();
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

class VaryingDataBuffer
{
public:
	void InitVerticesVaryingData(int vertexCount);
	VertexVaryingData& GetVertexVaryingData(int index);
	void InitDynamicVaryingData();
	void* CreateDynamicVaryingData();
	void ResetDynamicVaryingData();
	void InitPixelVaryingData();
	void* CreatePixelVaryingData();
	void ResetPixelVaryingData();

	bool SetVaryingDataDecl(std::vector<VaryingDataLayout> layout, int size);
	const VaryingDataDecl& GetVaryingDataDecl() { return varyingDataDecl; }

private:
	VaryingDataDecl varyingDataDecl;
	std::vector<VertexVaryingData> vertexVaryingData;
	Buffer vertexVaryingDataBuffer;
	Buffer dynamicVaryingDataBuffer;
	std::vector<PixelVaryingData> pixelVaryingData;
	Buffer pixelVaryingDataBuffer;
};

} // namespace rasterizer

#endif // !_RASTERIZER_RENDER_DATA_H_
