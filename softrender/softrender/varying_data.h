#ifndef _SOFTRENDER_VARYING_DATA_H_
#define _SOFTRENDER_VARYING_DATA_H_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "softrender/srtypes.hpp"
#include "softrender/buffer.h"

namespace sr
{

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
	static rawptr_t TriangleInterp(int slot, const VertexVaryingData& v0, const VertexVaryingData& v1, const VertexVaryingData& v2, float x, float y, float z);

	static void LinearInterpValue(rawptr_t output, const rawptr_t a, const rawptr_t b, int size, float t);
	static void TriangleInterpValue(rawptr_t output, const rawptr_t a, const rawptr_t b, const rawptr_t c, int size, float x, float y, float z);
};

class VaryingDataBuffer
{
public:
	VaryingDataBuffer() = default;

	void InitVaryingDataBuffer(int varyingDataSize);
	void InitVerticesVaryingData(int vertexCount);
	VertexVaryingData& GetVertexVaryingData(int index);
	void InitDynamicVaryingData();
	rawptr_t CreateDynamicVaryingData();
	void ResetDynamicVaryingData();
	void InitPixelVaryingData(int slot);
	VertexVaryingData& GetPixelVaryingData(int slot);

	int GetVaryingDataSize() const;

private:
	int varyingDataSize = 0;
	std::vector<VertexVaryingData> vertexVaryingData;
	std::vector<VertexVaryingData> pixelVaryingData;
	Buffer vertexVaryingDataBuffer;
	Buffer dynamicVaryingDataBuffer;
	Buffer pixelVaryingDataBuffer;
};

} // namespace sr

#endif // !_SOFTRENDER_VARYING_DATA_H_
