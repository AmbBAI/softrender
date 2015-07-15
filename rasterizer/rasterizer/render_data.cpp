#include "render_data.h"
using namespace rasterizer;

const uint32_t RenderData::VERTEX_MAX_COUNT = (1 << 16) - 1;

void VaryingDataBuffer::InitVerticesVaryingData(int vertexCount)
{
	int size = varyingDataDecl.size;
	vertexVaryingDataBuffer.Initialize(size, false);
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
	int size = varyingDataDecl.size;
	dynamicVaryingDataBuffer.Initialize(size, true);
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

void VaryingDataBuffer::InitPixelVaryingData()
{
	int size = varyingDataDecl.size;
	pixelVaryingDataBuffer.Initialize(size, true);
	pixelVaryingDataBuffer.Alloc(1);
}

rawptr_t VaryingDataBuffer::CreatePixelVaryingData()
{
	return pixelVaryingDataBuffer.itor.Get();
}

void VaryingDataBuffer::ResetPixelVaryingData()
{
	pixelVaryingDataBuffer.itor.Seek(0);
}

bool VaryingDataBuffer::SetVaryingDataDecl(const std::vector<VaryingDataElement>& decl, int size)
{
	varyingDataDecl.decl = decl;
	varyingDataDecl.size = size;
	varyingDataDecl.positionOffset = -1;

	for (auto element : decl)
	{
		if (element.offset + VaryingDataDecl::GetFormatSize(element.format) > size)
		{
			assert(false);
			return false;
		}

		if (element.usage == VaryingDataDeclUsage_SVPOSITION
			&& element.format == VaryingDataDeclFormat_Vector4)
		{
			varyingDataDecl.positionOffset = element.offset;
		}
	}
	assert(varyingDataDecl.positionOffset >= 0);
	return true;
}

VertexVaryingData VertexVaryingData::LinearInterp(const VertexVaryingData& a, const VertexVaryingData& b, float t)
{
	assert(a.varyingDataBuffer != nullptr);
	assert(a.varyingDataBuffer == b.varyingDataBuffer);

	auto varyingDataBuffer = a.varyingDataBuffer;

	VertexVaryingData output(varyingDataBuffer);
	const VaryingDataDecl& decl = varyingDataBuffer->GetVaryingDataDecl();
	output.data = varyingDataBuffer->CreateDynamicVaryingData();
	assert(output.data != nullptr);
	decl.LinearInterp(output.data, a.data, b.data, t);
	output.position = *Buffer::Value<Vector4>(output.data, decl.positionOffset);
	output.clipCode = Clipper::CalculateClipCode(output.position);
	return output;
}

rawptr_t VertexVaryingData::TriangleInterp(const VertexVaryingData& v0, const VertexVaryingData& v1, const VertexVaryingData& v2, float x, float y, float z)
{
	assert(v0.varyingDataBuffer != nullptr);
	assert(v0.varyingDataBuffer == v1.varyingDataBuffer && v0.varyingDataBuffer == v2.varyingDataBuffer);

	auto varyingDataBuffer = v0.varyingDataBuffer;
	const VaryingDataDecl& varyingDecl = varyingDataBuffer->GetVaryingDataDecl();
	rawptr_t data = varyingDataBuffer->CreatePixelVaryingData();
	assert(data != nullptr);
	varyingDecl.TriangleInterp(data, v0.data, v1.data, v2.data, x, y, z);
	return data;
}

template<typename Type>
void LinearInterpValue(rawptr_t output, const rawptr_t a, const rawptr_t b, int offset, float t)
{
	*Buffer::Value<Type>(output, offset) = Mathf::LinearInterp(*Buffer::Value<Type>(a, offset), *Buffer::Value<Type>(b, offset), t);
}

template<typename Type>
void TriangleInterpValue(rawptr_t output, const rawptr_t a, const rawptr_t b, const rawptr_t c, int offset, float x, float y, float z)
{
	*Buffer::Value<Type>(output, offset) = Mathf::TriangleInterp(*Buffer::Value<Type>(a, offset), *Buffer::Value<Type>(b, offset), *Buffer::Value<Type>(c, offset), x, y, z);
}

void VaryingDataDecl::LinearInterp(rawptr_t output, const rawptr_t a, const rawptr_t b, float t) const
{
	for (auto element : decl)
	{
		switch (element.format)
		{
		case VaryingDataDeclFormat_Float:
			LinearInterpValue<float>(output, a, b, element.offset, t);
			break;
		case VaryingDataDeclFormat_Vector2:
			LinearInterpValue<Vector2>(output, a, b, element.offset, t);
			break;
		case VaryingDataDeclFormat_Vector3:
			LinearInterpValue<Vector3>(output, a, b, element.offset, t);
			break;
		case VaryingDataDeclFormat_Vector4:
			LinearInterpValue<Vector4>(output, a, b, element.offset, t);
			break;
		default:
			break;
		}
	}
}

void VaryingDataDecl::TriangleInterp(rawptr_t output, const rawptr_t a, const rawptr_t b, const rawptr_t c, float x, float y, float z) const
{
	for (auto l : decl)
	{
		if (l.usage == VaryingDataDeclUsage_SVPOSITION) continue;
		switch (l.format)
		{
		case VaryingDataDeclFormat_Float:
			TriangleInterpValue<float>(output, a, b, c, l.offset, x, y, z);
			break;
		case VaryingDataDeclFormat_Vector2:
			TriangleInterpValue<Vector2>(output, a, b, c, l.offset, x, y, z);
			break;
		case VaryingDataDeclFormat_Vector3:
			TriangleInterpValue<Vector3>(output, a, b, c, l.offset, x, y, z);
			break;
		case VaryingDataDeclFormat_Vector4:
			TriangleInterpValue<Vector4>(output, a, b, c, l.offset, x, y, z);
			break;
		default:
			break;
		}
	}
}
