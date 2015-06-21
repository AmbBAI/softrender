#include "render_data.h"
using namespace rasterizer;

const u32 RenderData::VERTEX_MAX_COUNT = (1 << 16) - 1;

void* RenderData::CreateVertexBuff(u32 count, u32 size)
{
	vertexCount = count;
	vertexSize = size;

	vertexBuff.assign(count * size, 0);
	return (void*)(&vertexBuff[0]);
}

u16* RenderData::CreateIndexBuff(u32 count)
{
	indexCount = count;

	indexBuff.assign(count, 0);
	return &indexBuff[0];
}

void* RenderData::GetVertexData(u32 index)
{
	u32 offset = index * vertexSize;
	assert(offset < vertexBuff.size());
	assert(offset + vertexSize < vertexBuff.size());
	if (offset >= vertexBuff.size()) return nullptr;
	if (offset + vertexSize >= vertexBuff.size()) return nullptr;
	return (void*)&vertexBuff[offset];
}

void rasterizer::RenderData::CreateVaryingData()
{
	int size = decl.GetDataSize();
	
}

