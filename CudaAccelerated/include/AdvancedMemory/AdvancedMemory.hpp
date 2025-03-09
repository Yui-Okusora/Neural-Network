#pragma once
#include <Windows.h>

template<typename T>
T &derefPtr(void* ptr, size_t index = 0)
{
	return *(reinterpret_cast<T*>(ptr) + index);
}

class AdvancedMemory 
{
public:
	friend class MemoryManager;
	void load(unsigned long offset, unsigned long size); // offset and size in bytes
	void reserve(const size_t& fileSize); // in bytes
	LPVOID getMapPtr() { return lpMapAddress; }
	void* getViewPtr() { return (char*)lpMapAddress + iViewDelta; }
	const DWORD getFileSize() { return dwFileSize; }
	template<typename T>
	T& at(const size_t &index)
	{
		if (index >= (dwMapViewSize - iViewDelta) / sizeof(T)) throw std::out_of_range("Index out of range");
		return *(reinterpret_cast<T*>(getViewPtr()) + index);
	}
	~AdvancedMemory();
private:
	HANDLE hMapFile = NULL;      // handle for the file's memory-mapped region
	HANDLE hFile = NULL;         // the file handle
	DWORD dwFileSize = 0;     // temporary storage for file sizes
	DWORD dwMapViewSize = 0;  // the size of the view 
	LPVOID lpMapAddress = NULL;
	int iViewDelta = 0;
};


