#include "AdvancedMemory.hpp"
#include "./MemoryManager/MemoryManager.hpp"
#include <stdio.h>

void AdvancedMemory::load(unsigned long offset, unsigned long size)
{
	MemMng.getUsedMemory() -= dwMapViewSize;

	DWORD dwFileMapStart // where to start the file map view
	= (offset / MemMng.getSysGranularity()) * MemMng.getSysGranularity();

	dwMapViewSize = (offset % MemMng.getSysGranularity()) + size;
	iViewDelta = offset - dwFileMapStart;

	if (lpMapAddress != NULL) UnmapViewOfFile(lpMapAddress);

    lpMapAddress = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS, 0, dwFileMapStart, dwMapViewSize);
	
	if (lpMapAddress == NULL)
		printf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());

	MemMng.getUsedMemory() += dwMapViewSize;
}

void AdvancedMemory::reserve(const size_t& fileSize)
{
	DWORD dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
	if(hMapFile != NULL) CloseHandle(hMapFile);
	hMapFile = CreateFileMapping(hFile, NULL, 0, dwHigh32bSize, dwLow32bSize, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
}

AdvancedMemory::~AdvancedMemory()
{
	//printf("object destroyed");
	MemMng.getUsedMemory() -= dwMapViewSize;
	if (lpMapAddress != NULL) UnmapViewOfFile(lpMapAddress);
	if (hMapFile != NULL) CloseHandle(hMapFile);
	if (hFile != NULL) CloseHandle(hFile);
}

