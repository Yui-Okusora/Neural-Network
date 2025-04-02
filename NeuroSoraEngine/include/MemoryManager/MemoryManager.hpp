#pragma once

#define NeuroSoraCore_MemoryManager

#include "NeuroSoraCore.hpp"

#include "AdvancedMemory/AdvancedMemory.hpp"

class AdvancedMemory;

class MemoryManager {
public:
	static MemoryManager& getManager();
	void initManager();
	void setTmpDir(const std::string& dir);

	MemoryManager(MemoryManager const&) = delete;
	void operator=(MemoryManager const&) = delete;

	void createTmp(AdvancedMemory*& memPtr, const size_t& fileSize);
	void createPmnt(AdvancedMemory* memPtr, const size_t& fileSize);
	void memcopy(AdvancedMemory*& _dst, void* _src, const size_t& _size);
	void memcopy(AdvancedMemory*& _dst, AdvancedMemory* _src, const short& _typeSize, const size_t& _size);
	void move(AdvancedMemory* _dst, AdvancedMemory* _src);
	void free(AdvancedMemory* ptr);
	void addTmpInactive(const unsigned long& id) { inactiveFileID.emplace_back(id); }
	DWORD getSysGranularity() const { return dwSysGran; }
	std::atomic<unsigned long long>& getUsedMemory() { return usedMem; }
private:
	MemoryManager() {};
	void copyThreadsRawPtr(AdvancedMemory* _dst, void* _src, size_t offset, size_t _size);
	
	
	

	unsigned n_FileCreated = 0;
	DWORD dwSysGran = 0;
	std::string tmpDir = "";
	//char* permDir = "";
	std::atomic<unsigned long long> usedMem;
	std::atomic<unsigned long> fileID;
	std::atomic<unsigned long> permFileID;
	std::unique_ptr<std::shared_mutex> mutex = std::make_unique<std::shared_mutex>();
	std::list<unsigned long> inactiveFileID;
	//std::queue<unsigned long> inactivePermFileID;
	std::list<AdvancedMemory*> filePool;
};

static MemoryManager& MemMng = MemoryManager::getManager();
