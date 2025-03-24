#include "AdvancedMemory.hpp"
#include "./MemoryManager/MemoryManager.hpp"
#include <stdio.h>

ViewOfAdvancedMemory& AdvancedMemory::load(size_t offset, size_t size)
{
	ViewOfAdvancedMemory view;
	if (hFile == NULL)
		system("pause");
	if (hMapFile == NULL) 
		resize(size);
	if (offset >= dwFileSize) return view;
	if (offset + size >= dwFileSize) size = dwFileSize - offset;

	DWORD dwFileMapStart // where to start the file map view
	= (offset / MemMng.getSysGranularity()) * MemMng.getSysGranularity();

	view._offset = offset;
	view.dwMapViewSize = (offset % MemMng.getSysGranularity()) + size;
	view.iViewDelta = offset - dwFileMapStart;

	DWORD dwHighFileMapStart = (dwFileMapStart >> 32);
	dwFileMapStart &= 0xFFFFFFFF;

    view.lpMapAddress = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS, dwHighFileMapStart, dwFileMapStart, view.dwMapViewSize);
	
	if (view.lpMapAddress == NULL) throw std::bad_alloc();//return view;

	MemMng.getUsedMemory() += view.dwMapViewSize;

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

void AdvancedMemory::createMapObj()
{
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
}

void AdvancedMemory::unload(ViewOfAdvancedMemory& view)
{
	if (views.count(view.lpMapAddress) == 0) return;
	MemMng.getUsedMemory() -= view.dwMapViewSize;
	view.dwMapViewSize = 0;
	view.iViewDelta = 0;
	view._offset = 0;
	
	UnmapViewOfFile(view.lpMapAddress);
	view.lpMapAddress = NULL;
	views.erase(view.lpMapAddress);
}

void AdvancedMemory::unloadAll()
{
	if (views.empty()) return;
	for (auto &view : views)
		unload(view.second);
}

void AdvancedMemory::closeAllPtr()
{	
	unloadAll();
	if (hMapFile != NULL){
		CloseHandle(hMapFile);
		hMapFile = NULL;
	}
	if (hFile != NULL) {
		CloseHandle(hFile);
		hFile = NULL;
	}
}

AdvancedMemory::~AdvancedMemory()
{
	closeAllPtr();
	MemMng.addTmpInactive(fileID);
	dwFileSize = 0;
	fileID = 0;
}

