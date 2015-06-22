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
	bool Initialize(int blockSize, bool isDynamic = true);
	void Finalize();

	bool Alloc(int blockCount);
	bool Realloc(int blockCount);
	void Dealloc();

	void* operator[](int idx);

	struct Iterator
	{
	private:
		Buffer* buffer;
		int index;

	public:
		void* operator->() const;

		void Seek(int index);
		void* Get() const;
		void* Next();
	} itor;

protected:
	bool AllocPage(int page);
	bool DeallocPage(int page);

private:
	std::vector<u8*> data;
	int blockPrePage = 0;
	int blockSize = 1;
	int pageSize = 0;

	bool isDynamicBuffer = true;
	int allocatedBlockCount = 0;
};


}

#endif //! _BASE_BUFFER_H_