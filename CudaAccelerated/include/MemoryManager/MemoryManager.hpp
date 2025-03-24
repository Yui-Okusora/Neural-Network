#pragma once

#include <Windows.h>
#include <stdint.h>
#include <queue>

class AdvancedMemory;

class MemoryManager {
public:
	static MemoryManager& getManager();
	void initManager();
	void setTmpDir(char* dir);

	MemoryManager(MemoryManager const&) = delete;
	void operator=(MemoryManager const&) = delete;

	void createTmp(AdvancedMemory* memPtr, const size_t& fileSize);
	void createPmnt(AdvancedMemory* memPtr, const size_t& fileSize);
	void memcopy(AdvancedMemory* _dst, void* _src, const size_t& _size);
	void memcopy(AdvancedMemory* _dst, AdvancedMemory* _src, const short& _typeSize, const size_t& _size);
	void move(AdvancedMemory* _dst, AdvancedMemory* _src);
	void free(AdvancedMemory* ptr);
	void addTmpInactive(const unsigned long& id) { inactiveFileID.push(id); }
	DWORD getSysGranularity() const { return dwSysGran; }
	unsigned long long& getUsedMemory() { return usedMem; }
private:
	MemoryManager() {};
	void copyThreadsRawPtr(AdvancedMemory* _dst, void* _src, size_t offset, size_t _size);
	unsigned long long usedMem = 0;
	unsigned n_FileCreated = 0;
	DWORD dwSysGran = 0;
	char* tmpDir = "";
	char* permDir = "";
	unsigned long fileID = 0;
	unsigned long permFileID = 0;
	std::queue<unsigned long> inactiveFileID;
	std::queue<unsigned long> inactivePermFileID;
};

static MemoryManager& MemMng = MemoryManager::getManager();
