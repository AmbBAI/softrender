#ifndef _RASTERIZER_VARYING_DATA_H_
#define _RASTERIZER_VARYING_DATA_H_

#include "base/header.h"
#include "rasterizer/vertex.hpp"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "rasterizer/buffer.h"

namespace rasterizer
{

enum VaryingDataDeclUsage
{
	VaryingDataDeclUsage_SVPOSITION,
	VaryingDataDeclUsage_POSITION,
	VaryingDataDeclUsage_NORMAL,
	VaryingDataDeclUsage_COLOR,
	VaryingDataDeclUsage_TEXCOORD,
};

enum VaryingDataDeclFormat
{
	VaryingDataDeclFormat_Float,
	VaryingDataDeclFormat_Vector2,
	VaryingDataDeclFormat_Vector3,
	VaryingDataDeclFormat_Vector4,
};

struct VaryingDataElement
{
	int offset;
	VaryingDataDeclUsage usage;
	VaryingDataDeclFormat format;
};

struct VaryingDataDecl
{
	std::vector<VaryingDataElement> decl;
	int size;
	int positionOffset = -1;

	static int GetFormatSize(VaryingDataDeclFormat format)
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

	void LinearInterp(rawptr_t output, const rawptr_t a, const rawptr_t b, float t) const;
	void TriangleInterp(rawptr_t output, const rawptr_t a, const rawptr_t b, const rawptr_t c, float x, float y, float z) const;
};

class VaryingDataBuffer;
struct VertexVaryingData
{
	VaryingDataBuffer* varyingDataBuffer = nullptr;

	rawptr_t data = nullptr;

	Vector4 position;
	uint32_t clipCode = 0x00;

	VertexVaryingData() = default;
	explicit VertexVaryingData(VaryingDataBuffer* _varyingDataBuffer) : varyingDataBuffer(_varyingDataBuffer) {}
	static VertexVaryingData LinearInterp(const VertexVaryingData& a, const VertexVaryingData& b, float t);
	static rawptr_t TriangleInterp(const VertexVaryingData& v0, const VertexVaryingData& v1, const VertexVaryingData& v2, float x, float y, float z);
};

class VaryingDataBuffer
{
public:
	VaryingDataBuffer() = default;

	void InitVerticesVaryingData(int vertexCount);
	VertexVaryingData& GetVertexVaryingData(int index);
	void InitDynamicVaryingData();
	rawptr_t CreateDynamicVaryingData();
	void ResetDynamicVaryingData();
	void InitPixelVaryingData();
	rawptr_t CreatePixelVaryingData();
	void ResetPixelVaryingData();

	bool SetVaryingDataDecl(const std::vector<VaryingDataElement>& layout, int size);
	const VaryingDataDecl& GetVaryingDataDecl() { return varyingDataDecl; }

private:
	VaryingDataDecl varyingDataDecl;
	std::vector<VertexVaryingData> vertexVaryingData;
	Buffer vertexVaryingDataBuffer;
	Buffer dynamicVaryingDataBuffer;
	Buffer pixelVaryingDataBuffer;
};

} // namespace rasterizer

#endif // !_RASTERIZER_VARYING_DATA_H_
