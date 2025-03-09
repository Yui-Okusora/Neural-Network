#pragma once

#include <Windows.h>
#include <stdint.h>

class AdvancedMemory;

class MemoryManager {
public:
	static MemoryManager& getManager();
	void initManager();
	MemoryManager(MemoryManager const&) = delete;
	void operator=(MemoryManager const&) = delete;

	void createTmp(AdvancedMemory* memPtr, const size_t& fileSize);
	AdvancedMemory createPmnt();
	void free(AdvancedMemory* ptr);
	const DWORD getSysGranularity() { return dwSysGran; }
	unsigned long long& getUsedMemory() { return usedMem; }
private:
	MemoryManager() {};
	unsigned long long usedMem = 0;
	unsigned n_FileCreated = 0;
	DWORD dwSysGran = 0;
	unsigned long fileID = 0;
};

static MemoryManager& MemMng = MemoryManager::getManager();
