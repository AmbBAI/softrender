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
struct VertexOutData
{
	RenderData* renderData;
	u32 clipCode = 0x00;
	void* data = nullptr;

	Vector4 position;
	static VertexOutData LinearInterp(const VertexOutData& a, const VertexOutData& b, float t);
};

struct PixelInData
{
	RenderData* renderData;
	void* data = nullptr;

	static PixelInData TriangleInterp(VertexOutData& v0, VertexOutData& v1, VertexOutData& v2, float x, float y, float z);
};

struct VaryingDataDecl
{
	enum VaryingDataDeclUsage
	{
		VaryingDataDeclType_POSITION,
		VaryingDataDeclType_NORMAL,
		VaryingDataDeclType_COLOR,
		VaryingDataDeclType_TEXCOORD,
	};

	enum VaryingDataDeclFormat
	{
		VaryingDataDeclFormat_Float,
		VaryingDataDeclFormat_Vector2,
		VaryingDataDeclFormat_Vector3,
		VaryingDataDeclFormat_Vector4,
	};

	struct VaryingDeclData
	{
		int offset;
		VaryingDataDeclUsage usage;
		VaryingDataDeclFormat format;
	};
	std::vector<VaryingDeclData> decl;
	int dataSize;

	void Initilize();
	int GetSize() { return dataSize; }

	void LinearInterp(void* output, const void* a, const void* b, float t) const;
	void TriangleInterp(void* output, const void* a, const void* b, const void* c, float x, float y, float z) const;
};

class RenderData
{
//	friend struct PixelInData;
//	friend struct VertexOutData;

public:
	static const u32 VERTEX_MAX_COUNT;

	template<typename VertexType>
	bool AssignVertexBuffer(std::vector<VertexType> vertices)
	{
		int count = (int)vertices.size();
		assert(count <= VERTEX_MAX_COUNT);
		if (count > VERTEX_MAX_COUNT) return false;
		vertexBuffer.Assign(vertices);
		return true;
	}

	u32 GetVertexCount() { return vertexCount; }

	bool AssignIndexBuffer(std::vector<u16> indices)
	{
		int count = (int)indices.size();
		indexBuffer.Assign(indices);
		return true;
	}
	u32 GetIndicesCount() { return indexCount; }

	void* GetVertexData(u32 index);

	void InitVertexOutData();
	VertexOutData& GetVertexOutData(int index);
	void InitDynamicVaryingData();
	void* CreateDynamicVaryingData();
	//VertexOutData* GetDyamicVaryingData(int index);

	const VaryingDataDecl& GetVaryingDataDecl() { return decl; }

private:
	Buffer vertexBuffer;
	u32 vertexCount = 0;
	u32 vertexSize = 0;

	Buffer indexBuffer;
	u32 indexCount = 0;

	VaryingDataDecl decl;
	std::vector<VertexOutData> vertexOutData;
	Buffer vertexVaryingDataBuffer;
	std::vector<PixelInData> pixelVaryingData;
	Buffer pixelVaryingDataBuffer;
};

} // namespace rasterizer

#endif // !_RASTERIZER_RENDER_DATA_H_
