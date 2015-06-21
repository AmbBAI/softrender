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

namespace rasterizer
{

template <typename PixelType>
struct RenderPixelData
{
	u32 meshIndex;
	u32 triangleIndex;

	u32 x, y;
	float depth;
	PixelType pixel;
	Vector2 ddx, ddy;
};

struct VaryingData
{
	u32 clipCode = 0x00;
	Projection projection;

	int dataID = -1;
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

	u32 GetDataSize();
	std::vector<VertexDeclData> decl;
};

//struct VertexDecl
//{
//	enum VertexDeclDataUsage
//	{
//		VertexDeclDataType_POSITION,
//		VertexDeclDataType_NORMAL,
//		VertexDeclDataType_COLOR,
//		VertexDeclDataType_TEXCOORD,
//	};
//
//	enum VertexDeclDataFormat
//	{
//		VertexDeclDataFormat_Float,
//		VertexDeclDataFormat_Vector2,
//		VertexDeclDataFormat_Vector3,
//		VertexDeclDataFormat_Vector4,
//	};
//
//	struct VertexDeclData
//	{
//		int offset;
//		VertexDeclDataUsage usage;
//		VertexDeclDataFormat format;
//	};
//
//	u32 GetVertexSize();
//	std::vector<VertexDeclData> decl;
//};

class RenderData
{
public:
	static const u32 VERTEX_MAX_COUNT;

	void* CreateVertexBuff(u32 count, u32 size);
	void* GetVertexData(u32 index);
	u32 GetVertexCount() { return vertexCount; }

	u16* CreateIndexBuff(u32 count);
	u32 GetIndicesCount() { return indexCount; }
	
	void CreateVaryingData();
	VaryingData& GetVaryingData(int index);
	int CreateDynamicVaryingData();
	VaryingData& GetDyamicVaryingData(int index);

private:
	std::vector<u8> vertexBuff;
	u32 vertexCount = 0;
	u32 vertexSize = 0;

	std::vector<u16> indexBuff;
	u32 indexCount = 0;

	VaryingDataDecl decl;
	std::vector<VaryingData> vertexVaryingData;
	std::vector<u8> vertexVaryingDataBuff;
	std::vector<VaryingData> dynamicVaryingData;
	std::vector<u8> dynamicVaryingDataBuff;
};


}

#endif // !_RASTERIZER_RENDER_DATA_H_
