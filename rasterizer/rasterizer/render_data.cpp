#include "render_data.h"
using namespace rasterizer;

const u32 RenderData::VERTEX_MAX_COUNT = (1 << 16) - 1;

void* RenderData::GetVertexData(u32 index)
{
	return vertexBuffer[index];
}

void RenderData::InitVertexOutData()
{
	int size = decl.GetSize();

	vertexVaryingDataBuffer.Initialize(size, false);
	vertexVaryingDataBuffer.Alloc(vertexCount);
}

VertexOutData& RenderData::GetVertexOutData(int index)
{
	return vertexOutData[index];
}

void RenderData::InitDynamicVaryingData()
{
	int size = decl.GetSize();
	pixelVaryingDataBuffer.Initialize(size, true);
}

void* RenderData::CreateDynamicVaryingData()
{
	auto val = pixelVaryingDataBuffer.itor.Get();
	pixelVaryingDataBuffer.itor.Next();
	return val;
}

VertexOutData VertexOutData::LinearInterp(const VertexOutData& a, const VertexOutData& b, float t)
{
	assert(a.renderData != nullptr);
	assert(a.renderData == b.renderData);

	auto renderData = a.renderData;

	VertexOutData output;
	auto decl = renderData->GetVaryingDataDecl();
	output.data = renderData->CreateDynamicVaryingData();
	decl.LinearInterp(output.data, a.data, b.data, t);
	// TODO
	return output;
}

PixelInData PixelInData::TriangleInterp(VertexOutData& v0, VertexOutData& v1, VertexOutData& v2, float x, float y, float z)
{
	assert(v0.renderData != nullptr);
	assert(v0.renderData == v1.renderData && v0.renderData == v2.renderData);

	auto renderData = v0.renderData;

	PixelInData output;
	auto decl = renderData->GetVaryingDataDecl();
	output.data = renderData->CreateDynamicVaryingData();
	decl.TriangleInterp(output.data, v0.data, v1.data, v2.data, x, y, z);
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
	for (auto d : decl)
	{
		switch (d.format)
		{
		case VaryingDataDeclFormat_Float:
			LinearInterpValue<float>(output, a, b, d.offset, t);
			break;
		case VaryingDataDeclFormat_Vector2:
			LinearInterpValue<Vector2>(output, a, b, d.offset, t);
			break;
		case VaryingDataDeclFormat_Vector3:
			LinearInterpValue<Vector3>(output, a, b, d.offset, t);
			break;
		case VaryingDataDeclFormat_Vector4:
			LinearInterpValue<Vector4>(output, a, b, d.offset, t);
			break;
		default:
			break;
		}
	}
}

void VaryingDataDecl::TriangleInterp(void* output, const void* a, const void* b, const void* c, float x, float y, float z) const
{
	for (auto d : decl)
	{
		switch (d.format)
		{
		case VaryingDataDeclFormat_Float:
			TriangleInterpValue<float>(output, a, b, c, d.offset, x, y, z);
			break;
		case VaryingDataDeclFormat_Vector2:
			TriangleInterpValue<Vector2>(output, a, b, c, d.offset, x, y, z);
			break;
		case VaryingDataDeclFormat_Vector3:
			TriangleInterpValue<Vector3>(output, a, b, c, d.offset, x, y, z);
			break;
		case VaryingDataDeclFormat_Vector4:
			TriangleInterpValue<Vector4>(output, a, b, c, d.offset, x, y, z);
			break;
		default:
			break;
		}
	}
}
