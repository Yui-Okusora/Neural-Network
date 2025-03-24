#pragma once
#include <Windows.h>
#include <unordered_map>

struct ViewOfAdvancedMemory
{
	LPVOID lpMapAddress = NULL;		// first address of the mapped view
	DWORD dwMapViewSize = 0;		// the size of the view
	unsigned long iViewDelta = 0;
	unsigned long long _offset = 0;
};

class AdvancedMemory 
{
public:
	friend class MemoryManager;

	ViewOfAdvancedMemory& load(size_t offset, size_t size); // offset and size in bytes
	void unload(ViewOfAdvancedMemory& view);
	void unloadAll();
	void resize(const size_t& fileSize); // in bytes
	void createMapObj();
	//LPVOID getMapPtr() const { return lpMapAddress; }
	void* getViewPtr(const ViewOfAdvancedMemory& view) const { return (char*)view.lpMapAddress + view.iViewDelta; }
	DWORD getFileSize() const { return dwFileSize; }
	unsigned long getID() const { return fileID; }
	//DWORD getViewSize() const { return dwMapViewSize; }
	template<typename T> T& at(const size_t& index, const ViewOfAdvancedMemory &view) // recommended using
	{
		if (index >= (static_cast<size_t>(view.dwMapViewSize)-view.iViewDelta) / sizeof(T)) throw std::out_of_range("Index out of range");
		return *(reinterpret_cast<T*>(getViewPtr(view)) + index);
	}
	~AdvancedMemory();
private:
	void closeAllPtr();
	HANDLE hMapFile = NULL;      // handle for the file's memory-mapped region
	HANDLE hFile = NULL;         // the file handle
	DWORD dwFileSize = 0;     // temporary storage for file sizes  
	unsigned long fileID = 0;
	std::unordered_map<LPVOID, ViewOfAdvancedMemory> views;
};





