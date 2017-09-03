#include "varying_data.h"
using namespace sr;

void VaryingDataBuffer::InitVerticesVaryingData(int vertexCount)
{
	vertexVaryingDataBuffer.Initialize(varyingDataSize, false);
	vertexVaryingDataBuffer.Alloc(vertexCount);

	vertexVaryingData.assign(vertexCount, VertexVaryingData(this));
	for (int i = 0; i < vertexCount; ++i)
	{
		vertexVaryingData[i].data = vertexVaryingDataBuffer[i];
	}
}

VertexVaryingData& VaryingDataBuffer::GetVertexVaryingData(int index)
{
	return vertexVaryingData[index];
}

void VaryingDataBuffer::InitDynamicVaryingData()
{
	dynamicVaryingDataBuffer.Initialize(varyingDataSize, true);
	dynamicVaryingDataBuffer.Alloc(1);
}

rawptr_t VaryingDataBuffer::CreateDynamicVaryingData()
{
	return dynamicVaryingDataBuffer.itor.Get();
}

void VaryingDataBuffer::ResetDynamicVaryingData()
{
	dynamicVaryingDataBuffer.itor.Seek(0);
}

void VaryingDataBuffer::InitPixelVaryingData(int slot)
{
	pixelVaryingDataBuffer.Initialize(varyingDataSize, false);
	pixelVaryingDataBuffer.Alloc(slot);

	pixelVaryingData.assign(slot, VertexVaryingData(this));
	for (int i = 0; i < slot; ++i)
	{
		pixelVaryingData[i].data = pixelVaryingDataBuffer[i];
	}
}

void VaryingDataBuffer::InitVaryingDataBuffer(int varyingDataSize)
{
	this->varyingDataSize = varyingDataSize;
}

int VaryingDataBuffer::GetVaryingDataSize() const
{
	return varyingDataSize;
}

VertexVaryingData& VaryingDataBuffer::GetPixelVaryingData(int slot)
{
	assert(slot >= 0 && slot < (int)pixelVaryingData.size());
	return pixelVaryingData[slot];
}

VertexVaryingData VertexVaryingData::LinearInterp(const VertexVaryingData& a, const VertexVaryingData& b, float t)
{
	assert(a.varyingDataBuffer != nullptr);
	assert(a.varyingDataBuffer == b.varyingDataBuffer);

	auto varyingDataBuffer = a.varyingDataBuffer;

	VertexVaryingData output(varyingDataBuffer);
	output.data = varyingDataBuffer->CreateDynamicVaryingData();
	assert(output.data != nullptr);
	LinearInterpValue(output.data, a.data, b.data, varyingDataBuffer->GetVaryingDataSize(), t);
	output.position = *Buffer::Value<Vector4>(output.data, 0);
	output.clipCode = Clipper::CalculateClipCode(output.position);
	return output;
}

rawptr_t VertexVaryingData::TriangleInterp(int slotIndex, const VertexVaryingData& v0, const VertexVaryingData& v1, const VertexVaryingData& v2, float x, float y, float z)
{
	assert(v0.varyingDataBuffer != nullptr);
	assert(v0.varyingDataBuffer == v1.varyingDataBuffer && v0.varyingDataBuffer == v2.varyingDataBuffer);

	auto varyingDataBuffer = v0.varyingDataBuffer;
	rawptr_t data = varyingDataBuffer->GetPixelVaryingData(slotIndex).data;
	assert(data != nullptr);
	TriangleInterpValue(data, v0.data, v1.data, v2.data, varyingDataBuffer->GetVaryingDataSize(), x, y, z);
	return data;
}

void VertexVaryingData::LinearInterpValue(rawptr_t output, const rawptr_t a, const rawptr_t b, int size, float t)
{
	int offset = 0;
	while (offset < size)
	{
		*Buffer::Value<float>(output, offset) = Mathf::LinearInterp(*Buffer::Value<float>(a, offset), *Buffer::Value<float>(b, offset), t);
		offset += sizeof(float);
	}
}

void VertexVaryingData::TriangleInterpValue(rawptr_t output, const rawptr_t a, const rawptr_t b, const rawptr_t c, int size, float x, float y, float z)
{
	int offset = 0;
	while (offset < size)
	{
		*Buffer::Value<float>(output, offset) = Mathf::TriangleInterp(*Buffer::Value<float>(a, offset), *Buffer::Value<float>(b, offset), *Buffer::Value<float>(c, offset), x, y, z);
		offset += sizeof(float);
	}
}
