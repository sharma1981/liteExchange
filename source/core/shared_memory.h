#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

#include <core/memory/virtual_memory.h>

#ifdef __linux__
#elif _WIN32
#include <windows.h>
#endif

#include <string>
#include <cstddef>


namespace core
{

class SharedMemory
{
	public:
		SharedMemory();
		~SharedMemory();

		bool open(std::string name, std::size_t maxSize = DEFAULT_VIRTUAL_MEMORY_PAGE_SIZE, bool createFile = false, bool ipc = false, bool buffered = false);
		void write(void* buffer , std::size_t size);
		void close();

		std::size_t getSize() const { return m_size;  }
		bool isOpen() const
		{
			bool ret = false;
#ifdef __linux__
			ret = m_buffer != nullptr;
#elif _WIN32
			ret = m_handle != INVALID_HANDLE_VALUE;
#endif
			return ret;
		}

	private :
		char* m_buffer;
		std::size_t m_size;
		std::size_t m_writtenSize;
#ifdef __linux__
		int m_fileDescriptor;
#elif _WIN32
		HANDLE m_handle;
		HANDLE m_fileHandle;
#endif
};

} // namespace

#endif