#ifndef _BASE_BUFFER_H_
#define _BASE_BUFFER_H_

#include "base/header.h"

namespace rasterizer
{

class Buffer
{
public:
	static const int PAGE_SIZE_MAX;

	Buffer(int pageSize = 0);
	~Buffer();

public:
	bool Initialize(int blockSize, bool isDynamic = false);
	void Finalize();

	template<typename BlockType>
	bool Assign(const std::vector<BlockType>& datas);

	bool Alloc(int blockCount);
	bool Realloc(int blockCount);
	void Dealloc();

	void* operator[](int idx);

	struct Iterator
	{
	private:
		friend class Buffer;
		Buffer* buffer = nullptr;
		int index = 0;

	public:
		void Seek(int index) { this->index = index; }
		void* Get()
		{
			assert(buffer != nullptr);
			void* ret = (*buffer)[index];
			index += 1;
			return ret;
		}

	} itor;

	template<typename Type>
	static Type* Value(const void* data, int offset);

protected:
	bool AllocPage(int pageIdx);
	bool DeallocPage(int pageIdx);

private:
	std::vector<u8*> data;
	int blockPrePage = 0;
	int blockSize = 1;
	int pageSize = 0;

	bool isDynamicBuffer = true;
	int allocatedBlockCount = 0;
};

template<typename Type>
Type* Buffer::Value(const void* data, int offset)
{
	return (Type*)((u8*)data + offset);
}

template<typename BlockType>
bool Buffer::Assign(const std::vector<BlockType>& datas)
{
	if (!Initialize(sizeof(BlockType))) return false;
	int totalBlockCount = (int)datas.size();
	if (!Alloc(totalBlockCount)) return false;

	for (int p = 0;; ++p)
	{
		int blockOffset = p * blockPrePage;
		if (blockOffset >= totalBlockCount) return true;
		int blockCount = totalBlockCount - blockOffset;
		if (blockCount > blockPrePage) blockCount = blockPrePage;
		memcpy(data[p], &datas[blockOffset], blockCount * blockSize);
	}
}


}

#endif //! _BASE_BUFFER_H_