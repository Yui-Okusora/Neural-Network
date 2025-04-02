#include "MemoryManager.hpp"

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
	usedMem = 0;
	fileID = 0;
	permFileID = 0;
}

void MemoryManager::setTmpDir(const std::string& dir)
{
	std::unique_lock<std::shared_mutex> lock(*MemMng.mutex);
	tmpDir = dir;
	CreateDirectory(dir.c_str(), NULL);
}

void MemoryManager::createTmp(AdvancedMemory*& memPtr, const size_t &fileSize)
{
	std::unique_lock<std::shared_mutex> lockFilePool(*MemMng.mutex);
	if (!filePool.empty())
	{
		memPtr = filePool.front();
		memPtr->resize_s(fileSize);
		filePool.pop_front();
		return;
	}
	lockFilePool.unlock();


	AdvancedMemory* tmp = new AdvancedMemory();

	std::unique_lock<std::shared_mutex> lock(*tmp->mutex);

	char dir[MAX_PATH] = "";
	memcpy(dir, tmpDir.c_str(), tmpDir.length() * sizeof(char));
	char id[50] = "";

	if (inactiveFileID.empty())
	{
		_itoa_s(fileID, id, 10);
		tmp->fileID = fileID;
		++fileID;
	}
	else {
		std::unique_lock<std::shared_mutex> lockIDqueue(*MemMng.mutex);
		unsigned long tmpID = inactiveFileID.front();
		_itoa_s(tmpID, id, 10);
		tmp->fileID = tmpID;
		inactiveFileID.pop_front();
	}

	strcat_s(id, ".tmpbin");
	strcat_s(dir, id);
	LPCSTR lpcTheFile = dir;
	
	tmp->hFile = CreateFile(lpcTheFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (tmp->hFile == INVALID_HANDLE_VALUE)
	{
		printf(TEXT("hFile is NULL\n"));
		printf(TEXT("Target file is %s\n"),
			lpcTheFile);
	}
	//CHAR lpBuff[MAX_PATH] = "";

	//GetFullPathName(lpcTheFile, MAX_PATH, lpBuff, NULL);

	//printf("\n");
	//printf(lpBuff);
	//printf("\n");

	tmp->resize(fileSize);

	memPtr = tmp;
}

void MemoryManager::createPmnt(AdvancedMemory* memPtr, const size_t& fileSize)
{
	AdvancedMemory& tmp = *memPtr;
}

void MemoryManager::memcopy(AdvancedMemory*& _dst, void* _src, const size_t& _size)
{
	std::shared_lock<std::shared_mutex> lockDst(*_dst->mutex);
	std::unique_lock<std::shared_mutex> lockSrc(*MemMng.mutex);
	if (_dst->hFile == NULL) MemMng.createTmp(_dst, _size);
	if (_dst->getFileSize() != _size) {
		_dst->resize(_size);
	}
	lockDst.unlock();
	const size_t chunkSize = static_cast<size_t>(floor(_size / 4.0f));//MemMng.getSysGranularity() * 1024 * 10;
	size_t remainingData = _size;
	const int numThreads = (int)ceil(_size / (double)chunkSize);
	std::vector<std::thread> threads(numThreads);
	for (int i = 0; i < numThreads; ++i)
	{
		size_t chunkCpy = min(chunkSize, remainingData);
		threads[0]= std::thread(&MemoryManager::copyThreadsRawPtr, this, _dst, _src, chunkSize * i, chunkCpy);
		remainingData -= chunkCpy;
	}
	for (auto& a : threads)
		if (a.joinable())
			a.join();
}

void MemoryManager::memcopy(AdvancedMemory*& _dst, AdvancedMemory* _src, const short& _typeSize, const size_t& _size)
{
	Timer timer("MemoryManager::memcopy");
	if (_dst == NULL) MemMng.createTmp(_dst, _size);
	std::unique_lock<std::shared_mutex> lockDst(*_dst->mutex);
	std::unique_lock<std::shared_mutex> lockSrc(*_src->mutex);
	
	if (_dst->hFile == NULL)
	{
		lockDst.unlock();
		MemMng.createTmp(_dst, _size);
		lockDst.lock();
	}

	_dst->unloadAll();
	_src->unloadAll();

	CHAR lpDstFileName[MAX_PATH] = "";
	CHAR lpSrcFileName[MAX_PATH] = "";

	GetFinalPathNameByHandle(_dst->hFile, lpDstFileName, MAX_PATH, FILE_NAME_NORMALIZED);
	GetFinalPathNameByHandle(_src->hFile, lpSrcFileName, MAX_PATH, FILE_NAME_NORMALIZED);

	_dst->closeAllPtr();
	_src->closeAllPtr();
	CopyFile(lpSrcFileName, lpDstFileName, false);
	_dst->hFile = CreateFile(lpDstFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (_dst->hFile == NULL)
		int e = GetLastError();
	
	_src->hFile = CreateFile(lpSrcFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	_dst->dwFileSize = GetFileSize(_dst->hFile, NULL);
	_dst->createMapObj();
	_src->createMapObj();
}

void MemoryManager::copyThreadsRawPtr(AdvancedMemory* _dst, void* _src, size_t offset, size_t _size)
{
	ViewOfAdvancedMemory& dstView = _dst->load_s(offset, _size);
	memcpy(_dst->getViewPtr_s(dstView), (char*)_src + offset, _size);
	_dst->unload_s(dstView);
}

void MemoryManager::move(AdvancedMemory* _dst, AdvancedMemory* _src)
{
	HANDLE thisProcess = GetCurrentProcess();
	_dst->closeAllPtr();

	DuplicateHandle(thisProcess, _src->hFile, thisProcess, &(_dst->hFile), 0, 0, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(thisProcess, _src->hMapFile, thisProcess, &(_dst->hMapFile), 0, 0, DUPLICATE_SAME_ACCESS);

	_dst->dwFileSize = _src->dwFileSize;
	_dst->fileID = _src->fileID;

	_src->closeAllPtr();
}

void MemoryManager::free(AdvancedMemory *ptr) {
	ptr->reset();
	filePool.push_back(ptr);
}