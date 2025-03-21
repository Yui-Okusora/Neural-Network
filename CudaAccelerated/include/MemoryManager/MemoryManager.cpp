#include "MemoryManager.hpp"
#include "./AdvancedMemory/AdvancedMemory.hpp"
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>

std::mutex copyMutex;

MemoryManager& MemoryManager::getManager()
{
	static MemoryManager instance;
	return instance;
}

void MemoryManager::initManager()
{
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;
}

void MemoryManager::createTmp(AdvancedMemory *memPtr, const size_t &fileSize)
{
	AdvancedMemory &tmp = *memPtr;
	char str[50] = "";
	_itoa_s(fileID, str, 10);
	strcat_s(str, ".tmpbin");
	LPCSTR lpcTheFile = str;
	
	tmp.hFile = CreateFile(lpcTheFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (tmp.hFile == INVALID_HANDLE_VALUE)
	{
		printf(TEXT("hFile is NULL\n"));
		printf(TEXT("Target file is %s\n"),
			lpcTheFile);
	}
	CHAR lpBuff[MAX_PATH] = "";

	GetFullPathName(lpcTheFile, MAX_PATH, lpBuff, NULL);

	printf("\n");
	printf(lpBuff);
	printf("\n");

	DWORD dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
	if (tmp.hMapFile != NULL) CloseHandle(tmp.hMapFile);
	tmp.hMapFile = CreateFileMapping(tmp.hFile, NULL, PAGE_READWRITE, dwHigh32bSize, dwLow32bSize, NULL);
	tmp.dwFileSize = GetFileSize(tmp.hFile, NULL);
	++fileID;
}

AdvancedMemory MemoryManager::createPmnt()
{
	AdvancedMemory tmp;
	return tmp;
}

void MemoryManager::memcopy(AdvancedMemory* _dst, void* _src, const size_t& _size)
{
	if (_dst->hFile == NULL) MemMng.createTmp(_dst, _size);
	if (_dst->getFileSize() != _size) {
		_dst->resize(_size);
	}
	const size_t chunkSize = floor(_size / 4.0);//MemMng.getSysGranularity() * 1024 * 10;
	size_t remainingData = _size;
	const int numThreads = (int)ceil(_size / (double)chunkSize);
	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; ++i)
	{
		size_t chunkCpy = min(chunkSize, remainingData);
		threads.push_back(std::thread(&MemoryManager::copyThreadsRawPtr, this, _dst, _src, chunkSize * i, chunkCpy));
		remainingData -= chunkCpy;
	}
	for (auto& a : threads)
		if (a.joinable())
			a.join();
	_dst->unloadAll();
}

void MemoryManager::memcopy(AdvancedMemory* _dst, AdvancedMemory* _src, const short& _typeSize, const size_t& _size)
{
	if (_dst->hFile == NULL) MemMng.createTmp(_dst, _size);
	if (_dst->getFileSize() != _size) {
		_dst->resize(_src->getFileSize());
	}

	_dst->unloadAll();
	_src->unloadAll();

	CHAR lpDstFileName[MAX_PATH] = "";
	CHAR lpSrcFileName[MAX_PATH] = "";

	GetFinalPathNameByHandle(_dst->hFile, lpDstFileName, MAX_PATH, FILE_NAME_NORMALIZED);
	GetFinalPathNameByHandle(_src->hFile, lpSrcFileName, MAX_PATH, FILE_NAME_NORMALIZED);

	_dst->closeAllPtr();
	_src->closeAllPtr();
	//DeleteFile(lpDstFileName);
	CopyFile(lpSrcFileName, lpDstFileName, false);
	_dst->hFile = CreateFile(lpDstFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	_src->hFile = CreateFile(lpSrcFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	_dst->dwFileSize = GetFileSize(_dst->hFile, NULL);
	_dst->createMapObj();
	_src->createMapObj();
}

void MemoryManager::copyThreadsRawPtr(AdvancedMemory* _dst, void* _src, size_t offset, size_t _size)
{
	ViewOfAdvancedMemory& dstView = _dst->load(offset, _size);
	memcpy(_dst->getViewPtr(dstView), (char*)_src + offset, _size);
	_dst->unload(dstView.lpMapAddress);
}

void MemoryManager::move(AdvancedMemory* _dst, AdvancedMemory* _src)
{
	HANDLE thisProcess = GetCurrentProcess();
	_dst->closeAllPtr();

	DuplicateHandle(thisProcess, _src->hFile, thisProcess, &(_dst->hFile), 0, 0, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(thisProcess, _src->hMapFile, thisProcess, &(_dst->hMapFile), 0, 0, DUPLICATE_SAME_ACCESS);
	_src->closeAllPtr();
}

void MemoryManager::free(AdvancedMemory *ptr) {

}