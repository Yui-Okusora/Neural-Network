#include "AdvancedMemory.hpp"

bool AdvancedMemory::isValid() const
{
	return (hFile != INVALID_HANDLE_VALUE) && (hMapFile != INVALID_HANDLE_VALUE);
}

ViewOfAdvancedMemory& AdvancedMemory::_load(ViewOfAdvancedMemory& view, size_t offset, size_t size)
{
	DWORD dwFileMapStart // where to start the file map view
		= static_cast<DWORD>((offset / MemMng.getSysGranularity()) * MemMng.getSysGranularity());

	view._offset = offset;
	view.dwMapViewSize = static_cast<DWORD>((offset % MemMng.getSysGranularity()) + size);
	view.iViewDelta = static_cast<unsigned long>(offset - dwFileMapStart);

	DWORD dwHighFileMapStart = (static_cast<size_t>(dwFileMapStart) >> 32);
	dwFileMapStart &= 0xFFFFFFFF;

	view.lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, dwHighFileMapStart, dwFileMapStart, view.dwMapViewSize);

	if (view.lpMapAddress == NULL)
		throw std::bad_alloc();

	MemMng.getUsedMemory() += view.dwMapViewSize;

	return view;
}

ViewOfAdvancedMemory& AdvancedMemory::load(size_t offset, size_t size)
{
	ViewOfAdvancedMemory view;

	if (hFile == NULL)
		throw std::invalid_argument("");
	if (hMapFile == NULL)
		resize(size);
	if (offset >= dwFileSize) return view;
	if (offset + size >= dwFileSize) size = dwFileSize - offset;

	_load(view, offset, size);

	views[view.lpMapAddress] = view;

	return views[view.lpMapAddress];
}

ViewOfAdvancedMemory& AdvancedMemory::load_s(size_t offset, size_t size)
{
	ViewOfAdvancedMemory view;

	std::shared_lock<std::shared_mutex> lock(*mutex);

	if (hFile == NULL)
		system("pause");
	if (hMapFile == NULL)
		resize(size);
	if (offset >= dwFileSize) return view;
	if (offset + size >= dwFileSize) size = dwFileSize - offset;
	lock.unlock();

	std::unique_lock<std::shared_mutex> lockLoad(*view.mutex);
	_load(view, offset, size);
	lockLoad.unlock();

	std::unique_lock<std::shared_mutex> lockMap(*mutex);
	views[view.lpMapAddress] = view;

	return views[view.lpMapAddress];
}

void AdvancedMemory::resize(const size_t& fileSize)
{
	unloadAll();
	if (hMapFile != NULL)
		CloseHandle(hMapFile);
	LONG dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
	SetFilePointer(hFile, dwLow32bSize, &dwHigh32bSize, FILE_BEGIN);
	SetEndOfFile(hFile);
	dwFileSize = GetFileSize(hFile, NULL);
	createMapObj();
}

void AdvancedMemory::resize_s(const size_t& fileSize)
{
	unloadAll();
	std::unique_lock<std::shared_mutex> lock(*mutex);
	if (hMapFile != NULL) 
		CloseHandle(hMapFile);
	LONG dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
	SetFilePointer(hFile, dwLow32bSize, &dwHigh32bSize, FILE_BEGIN);
	SetEndOfFile(hFile);
	dwFileSize = GetFileSize(hFile, NULL);
	lock.unlock();
	createMapObj();
}

void AdvancedMemory::createMapObj()
{
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
}

void AdvancedMemory::createMapObj_s()
{
	std::unique_lock<std::shared_mutex> lock(*mutex);
	createMapObj();
}

void AdvancedMemory::unload(ViewOfAdvancedMemory& view)
{
	if (views.count(view.lpMapAddress) == 0) return;
	MemMng.getUsedMemory() -= view.dwMapViewSize;
	view.dwMapViewSize = 0;
	view.iViewDelta = 0;
	view._offset = 0;

	UnmapViewOfFile(view.lpMapAddress);
	views.erase(view.lpMapAddress);
	view.lpMapAddress = NULL;
}

void AdvancedMemory::unload_s(ViewOfAdvancedMemory& view)
{
	std::unique_lock<std::shared_mutex> lock(*mutex);

	if (views.empty()) return;

	std::unique_lock<std::shared_mutex> lockView(*view.mutex);
	if (views.count(view.lpMapAddress) == 0) return;

	
	MemMng.getUsedMemory() -= view.dwMapViewSize;
	view.dwMapViewSize = 0;
	view.iViewDelta = 0;
	view._offset = 0;
	
	UnmapViewOfFile(view.lpMapAddress);
	views.erase(view.lpMapAddress);
	view.lpMapAddress = NULL;
}

void AdvancedMemory::unloadAll()
{
	if (views.empty()) return;
	for (auto& view : views)
		unload_s(view.second);
}

void AdvancedMemory::unloadAll_s()
{
	std::unique_lock<std::shared_mutex> lock(*mutex);
	if (views.empty()) return;
	for (auto& view : views)
	{
		std::unique_lock<std::shared_mutex> lockView(*view.second.mutex);
		unload(view.second);
	}
}

void AdvancedMemory::closeAllPtr()
{
	unloadAll();
	if (hMapFile != NULL) {
		CloseHandle(hMapFile);
		hMapFile = NULL;
	}
	if (hFile != NULL) {
		CloseHandle(hFile);
		hFile = NULL;
	}
}

void AdvancedMemory::closeAllPtr_s()
{	
	unloadAll_s();
	std::unique_lock<std::shared_mutex> lock(*mutex);
	if (hMapFile != NULL){
		CloseHandle(hMapFile);
		hMapFile = NULL;
	}
	if (hFile != NULL) {
		CloseHandle(hFile);
		hFile = NULL;
	}
}

void* AdvancedMemory::getViewPtr(const ViewOfAdvancedMemory& view) const
{
	return (char*)view.lpMapAddress + view.iViewDelta;
}

void* AdvancedMemory::getViewPtr_s(const ViewOfAdvancedMemory& view) const
{
	std::unique_lock<std::shared_mutex> lock(*view.mutex);
	return getViewPtr(view);
}

const DWORD& AdvancedMemory::getFileSize() const
{
	return dwFileSize;
}

const DWORD& AdvancedMemory::getFileSize_s() const 
{
	std::shared_lock<std::shared_mutex> lock(*mutex);
	return getFileSize();
}

unsigned long AdvancedMemory::getID() const
{
	return fileID;
}

unsigned long AdvancedMemory::getID_s() const
{
	std::shared_lock<std::shared_mutex> lock(*mutex);
	return getID();
}

void AdvancedMemory::reset()
{
	unloadAll_s();
	CloseHandle(hMapFile);
	hMapFile = NULL;
	dwFileSize = 0;
}

AdvancedMemory::~AdvancedMemory()
{
	closeAllPtr();
	std::unique_lock<std::shared_mutex> lock(*mutex);
	MemMng.addTmpInactive(fileID);
	dwFileSize = 0;
	fileID = 0;

}

