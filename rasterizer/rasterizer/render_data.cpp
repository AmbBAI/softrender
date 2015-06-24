#include "render_data.h"
using namespace rasterizer;

const u32 RenderData::VERTEX_MAX_COUNT = (1 << 16) - 1;

void VaryingDataBuffer::InitVerticesVaryingData(int vertexCount)
{
	int size = varyingDataDecl.size;
	vertexVaryingDataBuffer.Initialize(size, false);
	vertexVaryingDataBuffer.Alloc(vertexCount);

	vertexOutData.assign(vertexCount, VertexOutData(this));
	for (int i = 0; i < vertexCount; ++i)
	{
		vertexOutData[i].data = vertexVaryingDataBuffer[i];
	}
}

VertexOutData& VaryingDataBuffer::GetVertexVaryingData(int index)
{
	return vertexOutData[index];
}

void VaryingDataBuffer::InitDynamicVaryingData()
{
	int size = varyingDataDecl.size;
	pixelVaryingDataBuffer.Initialize(size, true);
}

void* VaryingDataBuffer::CreateDynamicVaryingData()
{
	auto val = pixelVaryingDataBuffer.itor.Get();
	pixelVaryingDataBuffer.itor.Next();
	return val;
}

bool VaryingDataBuffer::SetVaryingDataDecl(std::vector<VaryingDataDecl::Layout> layout, int size)
{
	varyingDataDecl.layout = layout;
	varyingDataDecl.size = size;

	for (auto l : layout)
	{
		if (l.offset + VaryingDataDecl::GetFormatSize(l.format) > size)
		{
			assert(false);
			return false;
		}

		if (l.usage == VaryingDataDecl::VaryingDataDeclUsage_POSITION
			&& l.format == VaryingDataDecl::VaryingDataDeclFormat_Vector4)
		{
			varyingDataDecl.positionOffset = l.offset;
		}
	}
	assert(varyingDataDecl.positionOffset >= 0);
	return true;
}

VertexOutData VertexOutData::LinearInterp(const VertexOutData& a, const VertexOutData& b, float t)
{
	assert(a.varyingDataBuffer != nullptr);
	assert(a.varyingDataBuffer == b.varyingDataBuffer);

	auto varyingDataBuffer = a.varyingDataBuffer;

	VertexOutData output(varyingDataBuffer);
	auto decl = varyingDataBuffer->GetVaryingDataDecl();
	output.data = varyingDataBuffer->CreateDynamicVaryingData();
	decl.LinearInterp(output.data, a.data, b.data, t);
	output.position = *Buffer::Value<Vector4>(output.data, decl.positionOffset);
	output.clipCode = Clipper::CalculateClipCode(output.position);
	return output;
}

PixelInData PixelInData::TriangleInterp(VertexOutData& v0, VertexOutData& v1, VertexOutData& v2, float x, float y, float z)
{
	assert(v0.varyingDataBuffer != nullptr);
	assert(v0.varyingDataBuffer == v1.varyingDataBuffer && v0.varyingDataBuffer == v2.varyingDataBuffer);

	auto varyingDataBuffer = v0.varyingDataBuffer;
	PixelInData output;
	auto varyingDecl = varyingDataBuffer->GetVaryingDataDecl();
	output.data = varyingDataBuffer->CreateDynamicVaryingData();
	varyingDecl.TriangleInterp(output.data, v0.data, v1.data, v2.data, x, y, z);
	// TODO
	return output;
}

template<typename Type>
void LinearInterpValue(void* output, const void* a, const void* b, int offset, float t)
{
	*Buffer::Value<Type>(output, offset) = Mathf::LinearInterp(*Buffer::Value<Type>(a, offset), *Buffer::Value<Type>(b, offset), t);
}

template<typename Type>
void TriangleInterpValue(void* output, const void* a, const void* b, const void* c, int offset, float x, float y, float z)
{
	*Buffer::Value<Type>(output, offset) = Mathf::TriangleInterp(*Buffer::Value<Type>(a, offset), *Buffer::Value<Type>(b, offset), *Buffer::Value<Type>(c, offset), x, y, z);
}

void VaryingDataDecl::LinearInterp(void* output, const void* a, const void* b, float t) const
{
	for (auto l : layout)
	{
		switch (l.format)
		{
		case VaryingDataDeclFormat_Float:
			LinearInterpValue<float>(output, a, b, l.offset, t);
			break;
		case VaryingDataDeclFormat_Vector2:
			LinearInterpValue<Vector2>(output, a, b, l.offset, t);
			break;
		case VaryingDataDeclFormat_Vector3:
			LinearInterpValue<Vector3>(output, a, b, l.offset, t);
			break;
		case VaryingDataDeclFormat_Vector4:
			LinearInterpValue<Vector4>(output, a, b, l.offset, t);
			break;
		default:
			break;
		}
	}
}

void VaryingDataDecl::TriangleInterp(void* output, const void* a, const void* b, const void* c, float x, float y, float z) const
{
	for (auto l : layout)
	{
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
