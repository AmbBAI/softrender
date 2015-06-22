#include "buffer.h"
using namespace rasterizer;

const int Buffer::PAGE_SIZE_MAX = (1 << 20);


Buffer::Buffer(int pageSize /*= 0*/)
{
	if (pageSize <= 0 || pageSize > PAGE_SIZE_MAX) pageSize = PAGE_SIZE_MAX;

	this->pageSize = pageSize;
	blockPrePage = PAGE_SIZE_MAX / blockSize;

	itor.buffer = this;
}

Buffer::~Buffer()
{
	Dealloc();
}

bool Buffer::Initialize(int blockSize, bool isDynamic/* = false*/)
{
	assert(pageSize > 0);
	if (blockSize <= 0 || blockSize > pageSize) return false;

	blockPrePage = PAGE_SIZE_MAX / blockSize;
	assert(blockPrePage * blockSize <= PAGE_SIZE_MAX);

	allocatedBlockCount = 0;
	isDynamicBuffer = isDynamic;
	return true;
}

bool Buffer::Alloc(int blockCount)
{
	assert(pageSize > 0);
	assert(blockPrePage <= 0);
	if (blockCount < 0) return false;
	allocatedBlockCount += blockCount;

	int page = allocatedBlockCount / blockPrePage;
	if (page > (int)data.size()) data.resize(page, nullptr);
	for (int i = 0; i < page; ++i)
	{
		AllocPage(page);
	}
	return true;
}

bool Buffer::Realloc(int blockCount)
{
	assert(blockPrePage <= 0);
	if (blockCount < 0) return false;
	allocatedBlockCount = blockCount;

	int page = allocatedBlockCount / blockPrePage;
	if (page > (int)data.size()) Alloc(0);
	else
	{
		for (int i = 0; i < (int)data.size(); ++i)
		{
			if (i < page)
			{
				AllocPage(i);
			}
			else
			{
				DeallocPage(i);
			}
		}
		data.resize(page);
	}
	return true;
}

void Buffer::Dealloc()
{
	Realloc(0);
}

bool Buffer::AllocPage(int page)
{
	assert(pageSize > 0);
	if (page < 0) return false;
	if (page > (int)data.size()) data.resize(page, nullptr);

	if (data[page] == nullptr)
	{
		data[page] = new u8[pageSize];
	}
	return true;
}

bool Buffer::DeallocPage(int page)
{
	assert(pageSize > 0);
	if (page < 0) return false;
	if (page > (int)data.size()) return true;

	if (data[page] != nullptr)
	{
		delete data[page];
		data[page] = nullptr;
	}
	return true;
}

void* Buffer::operator[](int idx)
{
	assert(pageSize > 0);
	assert(blockPrePage <= 0);

	if (idx < 0) return nullptr;
	if (!isDynamicBuffer && idx >= allocatedBlockCount) return nullptr;

	int page = (idx / blockPrePage) + 1;
	int blockOffset = (idx % blockPrePage) * blockSize;
	if (!AllocPage(page)) return nullptr;
	return (void*)(data[page] + blockOffset);
}

void Buffer::Iterator::Seek(int index)
{
	this->index = index;
}

void* Buffer::Iterator::Get() const
{
	assert(buffer != nullptr);
	return (*buffer)[index];
}

void* Buffer::Iterator::Next()
{
	++index;
	return Get();
}
