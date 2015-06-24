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
struct VertexOutData
{
	VaryingDataBuffer* varyingDataBuffer = nullptr;

	void* data = nullptr;

	Vector4 position;
	u32 clipCode = 0x00;

	VertexOutData() = default;
	VertexOutData(VaryingDataBuffer* _varyingDataBuffer) : varyingDataBuffer(_varyingDataBuffer) {}
	static VertexOutData LinearInterp(const VertexOutData& a, const VertexOutData& b, float t);
};

struct PixelInData
{
	RenderData* renderData;
	VaryingDataBuffer* varyingDataBuffer;
	void* data = nullptr;

	static PixelInData TriangleInterp(VertexOutData& v0, VertexOutData& v1, VertexOutData& v2, float x, float y, float z);
};

struct VaryingDataDecl
{
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

	struct Layout
	{
		int offset;
		VaryingDeclUsage usage;
		VaryingDeclFormat format;
	};
	std::vector<Layout> layout;
	int size;
	int positionOffset = -1;
	int texCoordOffset = -1;

	static int GetFormatSize(VaryingDeclFormat format)
	{
		switch (format)
		{
		case rasterizer::VaryingDataDecl::VaryingDataDeclFormat_Float:
			return 4;
		case rasterizer::VaryingDataDecl::VaryingDataDeclFormat_Vector2:
			return 8;
		case rasterizer::VaryingDataDecl::VaryingDataDeclFormat_Vector3:
			return 12;
		case rasterizer::VaryingDataDecl::VaryingDataDeclFormat_Vector4:
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

	u32 GetVertexCount() { return vertexCount; }

	bool AssignIndexBuffer(const std::vector<u16>& indices)
	{
		int count = (int)indices.size();
		indexBuffer.Assign(indices);
		indexCount = count;
		return true;
	}
	u32 GetIndicesCount() { return indexCount; }

	void* GetVertexData(u32 index){ return vertexBuffer[index]; }

private:
	Buffer vertexBuffer;
	u32 vertexCount = 0;
	u32 vertexSize = 0;

	Buffer indexBuffer;
	u32 indexCount = 0;
};

class VaryingDataBuffer
{
public:
	void InitVerticesVaryingData(int vertexCount);
	VertexOutData& GetVertexVaryingData(int index);
	void InitDynamicVaryingData();
	void* CreateDynamicVaryingData();
	//VertexOutData* GetDyamicVaryingData(int index);

	bool SetVaryingDataDecl(std::vector<VaryingDataDecl::Layout> layout, int size);
	const VaryingDataDecl& GetVaryingDataDecl() { return varyingDataDecl; }

private:
	VaryingDataDecl varyingDataDecl;
	std::vector<VertexOutData> vertexOutData;
	Buffer vertexVaryingDataBuffer;
	std::vector<PixelInData> pixelVaryingData;
	Buffer pixelVaryingDataBuffer;
};

} // namespace rasterizer

#endif // !_RASTERIZER_RENDER_DATA_H_
