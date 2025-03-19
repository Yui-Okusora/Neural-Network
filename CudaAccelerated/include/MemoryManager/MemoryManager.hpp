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
	void memcopy(AdvancedMemory* _dst, void* _src, const size_t& _size);
	void memcopy(AdvancedMemory* _dst, AdvancedMemory* _src, const short& _typeSize, const size_t& _size);
	void move(AdvancedMemory* _dst, AdvancedMemory* _src);
	void free(AdvancedMemory* ptr);
	DWORD getSysGranularity() const { return dwSysGran; }
	unsigned long long& getUsedMemory() { return usedMem; }
private:
	MemoryManager() {};
	void copyThreads(AdvancedMemory* _dst, AdvancedMemory* _src, size_t offset, size_t _size);
	void copyThreadsRawPtr(AdvancedMemory* _dst, void* _src, size_t offset, size_t _size);
	unsigned long long usedMem = 0;
	unsigned n_FileCreated = 0;
	DWORD dwSysGran = 0;
	unsigned long fileID = 0;
	unsigned long permFileID = 0;
};

static MemoryManager& MemMng = MemoryManager::getManager();
