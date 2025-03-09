#include "MemoryManager.hpp"
#include "./AdvancedMemory/AdvancedMemory.hpp"
#include <stdio.h>
#include <string>

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
	itoa(fileID, str, 10);
	LPCSTR lpcTheFile = strcat(str, ".tmpbin");
	
	tmp.hFile = CreateFile(lpcTheFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (tmp.hFile == INVALID_HANDLE_VALUE)
	{
		printf(TEXT("hFile is NULL\n"));
		printf(TEXT("Target file is %s\n"),
			lpcTheFile);
	}
	CHAR lpBuff[MAX_PATH] = "";

	GetFullPathName(lpcTheFile, MAX_PATH, lpBuff, NULL);

	printf(lpBuff);
	printf("\n");

	DWORD dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
	if (tmp.hMapFile != NULL) CloseHandle(tmp.hMapFile);
	tmp.hMapFile = CreateFileMapping(tmp.hFile, NULL, PAGE_READWRITE, dwHigh32bSize, dwLow32bSize, NULL);
	tmp.dwFileSize = GetFileSize(tmp.hFile, NULL);
}

AdvancedMemory MemoryManager::createPmnt()
{
	AdvancedMemory tmp;
	return tmp;
}

void MemoryManager::free(AdvancedMemory *ptr) {

}