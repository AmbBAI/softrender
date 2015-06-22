#include "render_data.h"
using namespace rasterizer;

const u32 RenderData::VERTEX_MAX_COUNT = (1 << 16) - 1;

void RenderData::CreateVertexBuffer(u32 count, u32 size)
{
	vertexCount = count;
	vertexSize = size;

	vertexBuffer.Initialize(size, false);
	vertexBuffer.Alloc(count);
}

void RenderData::CreateIndexBuffer(u32 count)
{
	indexCount = count;
	indexBuffer.Initialize(sizeof(u16), false);
	indexBuffer.Alloc(count);
}

void* RenderData::GetVertexData(u32 index)
{
	return vertexBuffer[index];
}

void rasterizer::RenderData::InitVertexOutData()
{
	int size = decl.GetSize();

	vertexVaryingDataBuffer.Initialize(size, false);
	vertexVaryingDataBuffer.Alloc(vertexCount);
}

