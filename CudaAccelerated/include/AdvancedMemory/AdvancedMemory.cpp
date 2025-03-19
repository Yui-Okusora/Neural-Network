#include "AdvancedMemory.hpp"
#include "./MemoryManager/MemoryManager.hpp"
#include <stdio.h>

ViewOfAdvancedMemory& AdvancedMemory::load(size_t offset, size_t size)
{
	ViewOfAdvancedMemory view;
	if (hMapFile == NULL) resize(size);
	if (offset >= dwFileSize) return view;
	if (offset + size >= dwFileSize) size = dwFileSize - offset;

	DWORD dwFileMapStart // where to start the file map view
	= (offset / MemMng.getSysGranularity()) * MemMng.getSysGranularity();

	view._offset = offset;
	view.dwMapViewSize = (offset % MemMng.getSysGranularity()) + size;
	view.iViewDelta = offset - dwFileMapStart;

	DWORD dwHighFileMapStart = ((size_t)dwFileMapStart >> 32);
	dwFileMapStart &= 0xFFFFFFFF;

    view.lpMapAddress = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS, dwHighFileMapStart, dwFileMapStart, view.dwMapViewSize);
	
	if (view.lpMapAddress == NULL) return view;

	usedMem += view.dwMapViewSize;

	MemMng.getUsedMemory() += view.dwMapViewSize;

	views[view.lpMapAddress] = view;

	return views[view.lpMapAddress];
}

void AdvancedMemory::resize(const size_t& fileSize)
{
	unloadAll();
	CloseHandle(hMapFile);
	LONG dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
	SetFilePointer(hFile, dwLow32bSize, &dwHigh32bSize, FILE_BEGIN);
	SetEndOfFile(hFile);

	createMapObj();
}

void AdvancedMemory::createMapObj()
{
	hMapFile = CreateFileMapping(hFile, NULL, 0, 0, 0, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
}

void AdvancedMemory::unload(LPVOID viewAddress)
{
	ViewOfAdvancedMemory &view = views.at(viewAddress);
	MemMng.getUsedMemory() -= view.dwMapViewSize;
	view.dwMapViewSize = 0;
	view.iViewDelta = 0;
	view._offset = 0;
	
	UnmapViewOfFile(viewAddress);
	view.lpMapAddress = NULL;
	views.erase(viewAddress);
}

void AdvancedMemory::unloadAll()
{
	if (views.empty()) return;
	for (auto &view : views)
		unload(view.first);
}

void AdvancedMemory::closeAllPtr()
{	
	unloadAll();
	if (hMapFile != NULL) CloseHandle(hMapFile);
	if (hFile != NULL) CloseHandle(hFile);
}

AdvancedMemory::~AdvancedMemory()
{
	//printf("object destroyed");
	closeAllPtr();
}

