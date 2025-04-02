#pragma once

#define NeuroSoraCore_AdvancedMemory

#include "NeuroSoraCore.hpp"

#include "MemoryManager/MemoryManager.hpp"

class ViewOfAdvancedMemory
{
public:
	friend class AdvancedMemory;
	LPVOID lpMapAddress = NULL;		// first address of the mapped view
	DWORD dwMapViewSize = 0;		// the size of the view
	unsigned long iViewDelta = 0;
	unsigned long long _offset = 0;
private:
	std::shared_ptr<std::shared_mutex> mutex = std::make_shared<std::shared_mutex>();
};

class AdvancedMemory 
{
public:
	friend class MemoryManager;

	ViewOfAdvancedMemory& load(size_t offset, size_t size); // offset and size in bytes
	void unload(ViewOfAdvancedMemory& view);
	void unloadAll();
	void resize(const size_t& fileSize); // in bytes
	inline void createMapObj();
	void* getViewPtr(const ViewOfAdvancedMemory& view) const;
	inline const DWORD& getFileSize() const;
	inline unsigned long getID() const;
	bool isValid() const;
	template<typename T> 
	T& refAt(const size_t& index, const ViewOfAdvancedMemory& view) // recommended using
	{
		if (index >= (static_cast<size_t>(view.dwMapViewSize) - view.iViewDelta) / sizeof(T)) {
			throw std::out_of_range("Index out of range");
		}
		return *(reinterpret_cast<T*>(getViewPtr(view)) + index);
	}

	~AdvancedMemory();


	//--------- Thread-safe methods ----------

	ViewOfAdvancedMemory& load_s(size_t offset, size_t size); // offset and size in bytes
	void unload_s(ViewOfAdvancedMemory& view);
	void unloadAll_s();
	void resize_s(const size_t& fileSize); // in bytes
	void createMapObj_s();
	void* getViewPtr_s(const ViewOfAdvancedMemory& view) const;
	const DWORD& getFileSize_s() const;
	unsigned long getID_s() const;
	template<typename T>
	T& refAt_s(const size_t& index, const ViewOfAdvancedMemory& view) // recommended using
	{
		std::unique_lock<std::shared_mutex> lock(*view.mutex);
		if (index >= (static_cast<size_t>(view.dwMapViewSize) - view.iViewDelta) / sizeof(T)) {
			lock.unlock();
			throw std::out_of_range("Index out of range");
		}
		lock.unlock();
		return *(reinterpret_cast<T*>(getViewPtr(view)) + index);
	}

	template<typename T>
	T readAt(const size_t& index, const ViewOfAdvancedMemory& view) const // recommended using
	{
		std::shared_lock<std::shared_mutex> lock(*view.mutex);
		if (index >= (static_cast<size_t>(view.dwMapViewSize) - view.iViewDelta) / sizeof(T)) {
			lock.unlock();
			throw std::out_of_range("Index out of range");
		}
		lock.unlock();
		return *(reinterpret_cast<T*>(getViewPtr(view)) + index);
	}
private:
	ViewOfAdvancedMemory& _load(ViewOfAdvancedMemory& view, size_t offset, size_t size);
	void closeAllPtr();
	void closeAllPtr_s();
	void reset();

	HANDLE hMapFile = NULL;     // handle for the file's memory-mapped region
	HANDLE hFile = NULL;        // the file handle
	DWORD dwFileSize = 0;		// temporary storage for file sizes  
	unsigned long fileID = 0;
	std::unordered_map<LPVOID, ViewOfAdvancedMemory> views;
	std::shared_ptr<std::shared_mutex> mutex = std::make_shared<std::shared_mutex>();
};





