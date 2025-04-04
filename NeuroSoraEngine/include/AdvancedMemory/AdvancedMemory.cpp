#include "AdvancedMemory.hpp"

namespace NeuroSoraCore
{

	bool AdvancedMemory::isValid() const
	{
		return (m_hFile != INVALID_HANDLE_VALUE) && (m_hMapFile != INVALID_HANDLE_VALUE);
	}

	ViewOfAdvancedMemory& AdvancedMemory::_load(ViewOfAdvancedMemory& view, size_t offset, size_t size)
	{
		// where to start the file map view
		DWORD dwFileMapStart = static_cast<DWORD>((offset / MemMng.getSysGranularity()) * MemMng.getSysGranularity());

		view.parent = this;

		view._offset = offset;
		view.dwMapViewSize = static_cast<DWORD>((offset % MemMng.getSysGranularity()) + size);
		view.iViewDelta = static_cast<unsigned long>(offset - dwFileMapStart);

		DWORD dwHighFileMapStart = (static_cast<size_t>(dwFileMapStart) >> 32);
		dwFileMapStart &= 0xFFFFFFFF;

		view.lpMapAddress = MapViewOfFile(getMapHandle(), FILE_MAP_ALL_ACCESS, dwHighFileMapStart, dwFileMapStart, view.dwMapViewSize);

		if (view.lpMapAddress == NULL)
			throw std::bad_alloc();

		MemMng.getUsedMemory() += view.dwMapViewSize;

		return view;
	}

	ViewOfAdvancedMemory& AdvancedMemory::load(size_t offset, size_t size)
	{
		ViewOfAdvancedMemory view;

		if (getFileHandle() == NULL)
			throw std::invalid_argument("");
		if (getMapHandle() == NULL)
			resize(size);
		if (offset >= dwFileSize) return view;
		if (offset + size >= dwFileSize) size = dwFileSize - offset;

		_load(view, offset, size);

		views[view.lpMapAddress] = view;

		return views[view.lpMapAddress];
	}

	ViewOfAdvancedMemory& AdvancedMemory::load_s(size_t offset, size_t size)
	{
		ViewOfAdvancedMemory view;

		std::shared_lock<std::shared_mutex> lock(*mutex);

		if (getFileHandle() == NULL)
			system("pause");
		if (getMapHandle() == NULL)
			resize(size);
		if (offset >= dwFileSize) return view;
		if (offset + size >= dwFileSize) size = dwFileSize - offset;
		lock.unlock();

		std::unique_lock<std::shared_mutex> lockLoad(*view.mutex);
		_load(view, offset, size);
		lockLoad.unlock();

		std::unique_lock<std::shared_mutex> lockMap(*mutex);
		views[view.lpMapAddress] = view;

		return views[view.lpMapAddress];
	}

	void AdvancedMemory::resize(const size_t& fileSize)
	{
		unloadAll();
		if (getMapHandle() != NULL)
			CloseHandle(getMapHandle());
		LONG dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
		SetFilePointer(getFileHandle(), dwLow32bSize, &dwHigh32bSize, FILE_BEGIN);
		SetEndOfFile(getFileHandle());
		dwFileSize = GetFileSize(getFileHandle(), NULL);
		createMapObj();
	}

	void AdvancedMemory::resize_s(const size_t& fileSize)
	{
		unloadAll();
		std::unique_lock<std::shared_mutex> lock(*mutex);
		if (getMapHandle() != NULL)
			CloseHandle(getMapHandle());
		LONG dwHigh32bSize = fileSize >> 32, dwLow32bSize = fileSize & 0xFFFFFFFF;
		SetFilePointer(getFileHandle(), dwLow32bSize, &dwHigh32bSize, FILE_BEGIN);
		SetEndOfFile(getFileHandle());
		dwFileSize = GetFileSize(getFileHandle(), NULL);
		lock.unlock();
		createMapObj();
	}

	void AdvancedMemory::createMapObj()
	{
		getMapHandle() = CreateFileMapping(getFileHandle(), NULL, PAGE_READWRITE, 0, 0, NULL);
	}

	void AdvancedMemory::createMapObj_s()
	{
		std::unique_lock<std::shared_mutex> lock(*mutex);
		createMapObj();
	}

	void AdvancedMemory::unload(ViewOfAdvancedMemory& view)
	{
		if (views.count(view.lpMapAddress) == 0) return;
		MemMng.getUsedMemory() -= view.dwMapViewSize;
		view.dwMapViewSize = 0;
		view.iViewDelta = 0;
		view._offset = 0;

		UnmapViewOfFile(view.lpMapAddress);
		views.erase(view.lpMapAddress);
		view.lpMapAddress = NULL;
	}

	void AdvancedMemory::unload_s(ViewOfAdvancedMemory& view)
	{
		std::unique_lock<std::shared_mutex> lock(*mutex);

		if (views.empty()) return;

		std::unique_lock<std::shared_mutex> lockView(*view.mutex);
		if (views.count(view.lpMapAddress) == 0) return;


		MemMng.getUsedMemory() -= view.dwMapViewSize;
		view.dwMapViewSize = 0;
		view.iViewDelta = 0;
		view._offset = 0;

		UnmapViewOfFile(view.lpMapAddress);
		views.erase(view.lpMapAddress);
	}

	void AdvancedMemory::unloadAll()
	{
		if (views.empty()) return;
		for (auto& view : views)
			unload(view.second);
	}

	void AdvancedMemory::unloadAll_s()
	{
		std::unique_lock<std::shared_mutex> lock(*mutex);
		if (views.empty()) return;
		for (auto& view : views)
		{
			std::unique_lock<std::shared_mutex> lockView(*view.second.mutex);
			unload(view.second);
		}
	}

	void AdvancedMemory::closeAllPtr()
	{
		unloadAll();
		if (getMapHandle() != NULL) {
			CloseHandle(getMapHandle());
			getMapHandle() = NULL;
		}
		if (getFileHandle() != NULL) {
			CloseHandle(getFileHandle());
			getFileHandle() = NULL;
		}
	}

	void AdvancedMemory::closeAllPtr_s()
	{
		unloadAll_s();
		std::unique_lock<std::shared_mutex> lock(*mutex);
		if (getMapHandle() != NULL) {
			CloseHandle(getMapHandle());
			getMapHandle() = NULL;
		}
		if (getFileHandle() != NULL) {
			CloseHandle(getFileHandle());
			getFileHandle() = NULL;
		}
	}

	void* AdvancedMemory::getViewPtr(const ViewOfAdvancedMemory& view) const
	{
		return (char*)view.lpMapAddress + view.iViewDelta;
	}

	void* AdvancedMemory::getViewPtr_s(const ViewOfAdvancedMemory& view) const
	{
		std::unique_lock<std::shared_mutex> lock(*view.mutex);
		return getViewPtr(view);
	}

	const DWORD& AdvancedMemory::getFileSize() const
	{
		return dwFileSize;
	}

	const DWORD& AdvancedMemory::getFileSize_s() const
	{
		std::shared_lock<std::shared_mutex> lock(*mutex);
		return getFileSize();
	}

	unsigned long AdvancedMemory::getID() const
	{
		return m_fileID;
	}

	unsigned long AdvancedMemory::getID_s() const
	{
		std::shared_lock<std::shared_mutex> lock(*mutex);
		return getID();
	}

	void AdvancedMemory::reset()
	{
		unloadAll_s();
		CloseHandle(getMapHandle());
		getMapHandle() = NULL;
		dwFileSize = 0;
	}

	AdvancedMemory::~AdvancedMemory()
	{
		closeAllPtr();
		std::unique_lock<std::shared_mutex> lock(*mutex);
		MemMng.addTmpInactive(m_fileID);
		dwFileSize = 0;
		m_fileID = 0;
	}
}