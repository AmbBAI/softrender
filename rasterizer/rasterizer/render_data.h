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


struct PixelInData
{
	void* data = nullptr;
};

struct VertexOutData
{
	u32 clipCode = 0x00;
	void* data = nullptr;

	Vector4* GetPosition();
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

	struct VertexDeclData
	{
		int offset;
		VaryingDataDeclUsage usage;
		VaryingDataDeclFormat format;
	};
	std::vector<VertexDeclData> decl;

	void Initilize();
	u32 GetSize();
	void Lerp(void* output, void* input1, void* input2, float t);
};

class RenderData
{
public:
	static const u32 VERTEX_MAX_COUNT;

	void CreateVertexBuffer(u32 count, u32 size);
	void* GetVertexData(u32 index);
	u32 GetVertexCount() { return vertexCount; }

	void CreateIndexBuffer(u32 count);
	u32 GetIndicesCount() { return indexCount; }
	
	void InitVertexOutData();
	VertexOutData* GetVertexOutData(int index);
	int CreateDynamicVaryingData();
	VertexOutData* GetDyamicVaryingData(int index);

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


}

#endif // !_RASTERIZER_RENDER_DATA_H_
