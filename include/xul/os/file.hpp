#pragma once

#include <assert.h>
#include <xul/std/vsnprintf.hpp>

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif


#if defined(BOOST_WINDOWS)

#include <xul/mswin/windows.hpp>

namespace xul {


/// windows file object
class file : private boost::noncopyable
{
public:
    typedef HANDLE native_type;

    explicit file(HANDLE hFile = INVALID_HANDLE_VALUE) : m_handle(hFile)
    {
    }
    ~file()
    {
        close();
    }

    HANDLE native_handle() const { return m_handle; }

    bool is_open() const { return m_handle != INVALID_HANDLE_VALUE; }

    void close()
    {
        if (is_open())
        {
            BOOL success = ::CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            if (!success)
            {
                //TRACEOUT("File::Close CloseHandle failed with error %d\n", ::GetLastError());
                assert(false);
            }
        }
    }

    bool open(LPCTSTR path, DWORD access, DWORD shareMode, DWORD creationDisposition)
    {
        assert(!is_open());
        close();
        m_handle = ::CreateFile(path, access, shareMode, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (is_open())
            return true;
        // since file maybe used in logger, use TRACE instead of LOG
        //TRACE("file::open CreateFile %s failed with error %d\n", path, ::GetLastError());
        return false;
    }

    bool open_reading(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE, DWORD creationDisposition = OPEN_EXISTING)
    {
        return this->open_read(path, shareMode, creationDisposition);
    }

    bool open_read(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE, DWORD creationDisposition = OPEN_EXISTING)
    {
        return this->open(path, GENERIC_READ, shareMode, creationDisposition);
    }

    bool open_write(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ, DWORD creationDisposition = OPEN_ALWAYS)
    {
        return this->open(path, GENERIC_WRITE, shareMode, creationDisposition);
    }

    bool open_writing(LPCTSTR path, DWORD shareMode = FILE_SHARE_READ, DWORD creationDisposition = OPEN_ALWAYS)
    {
        return this->open_writing(path, shareMode, creationDisposition);
    }

    bool open_appending(LPCTSTR path, DWORD shareMode = 0, DWORD creationDisposition = OPEN_ALWAYS)
    {
        return open_writing(path, shareMode, creationDisposition) && seek_from_end(0);
    }

    size_t read(void* buffer, size_t size)
    {
        assert(buffer != NULL && size > 0);
        assert(is_open());
        DWORD bytes = 0;
        if (::ReadFile(m_handle, buffer, size, &bytes, NULL))
        {
            //assert(bytes > 0);
            return bytes;
        }
        //TRACE("file::read ReadFile failed. size=%u, bytes=%u error=%d\n", size, bytes, ::GetLastError());
        return 0;
    }

    bool read_n(void* buffer, size_t size)
    {
        size_t len = read(buffer, size);
        assert(len <= size);
        return len == size;
    }

    size_t write(const void* data, size_t size)
    {
        DWORD count = 0;
        if (!::WriteFile(native_handle(), data, size, &count, NULL))
            return 0;
        return count;
    }

    template <typename StructT>
    bool write_struct(const StructT& buffer)
    {
        size_t size = write(&buffer, sizeof(StructT));
        assert(size <= sizeof(StructT));
        return size == sizeof(StructT);
    }

    template <typename StructT>
    bool read_struct(StructT& buffer)
    {
        return read_n(&buffer, sizeof(StructT));
    }

    unsigned char read_byte()
    {
        unsigned char ch = 0;
        size_t bytes = read(&ch, 1);
        assert(bytes == 1);
        return ch;
    }

    bool write_variant(const TCHAR* format, va_list args)
    {
        const int max_size = 8 * 1024;
        TCHAR buffer[max_size + 1];
        buffer[max_size] = '\0';
        int count = vsntprintf(buffer, max_size, format, args);
        if (count <= 0)
        {
            assert(false);
            return false;
        }
        return write(buffer, count) == static_cast<size_t>( count );
    }

    bool write_format(const TCHAR* format, ...)
    {
        va_list args;
        va_start(args, format);
        bool success = write_variant(format, args);
        va_end(args);
        return success;
    }

    bool flush()
    {
        return ::FlushFileBuffers(native_handle()) != FALSE;
    }

    bool seek(LONG distance, DWORD moveMethod)
    {
#ifndef INVALID_SET_FILE_POINTER
        const DWORD INVALID_SET_FILE_POINTER = -1;
#endif

        assert(is_open());
        DWORD result = ::SetFilePointer(m_handle, distance, NULL, moveMethod);
        if (result == INVALID_SET_FILE_POINTER)
        {
            DWORD errcode = ::GetLastError();
            //TRACE("file::seek SetFilePointer failed, result=%u,error=%d,distance=%d,move=%u\n", result, errcode, distance, moveMethod);
            assert(errcode != ERROR_SUCCESS);
            return false;
        }
        return true;
    }
    bool seek_from_beginning(long distance = 0)
    {
        return seek(distance, FILE_BEGIN);
    }
    bool seek_from_end(long distance = 0)
    {
        return seek(distance, FILE_END);
    }
    bool seek_offset(long distance)
    {
        return seek(distance, FILE_CURRENT);
    }

    DWORD get_size32()
    {
        return ::GetFileSize(m_handle, NULL);
    }

    bool set_end_of_file()
    {
        return ::SetEndOfFile(m_handle) != FALSE;
    }

#ifndef _WIN32_WCE
    INT64 get_size()
    {
        return get_size64();
    }

    INT64 get_size64()
    {
        LARGE_INTEGER size;
        size.QuadPart = 0;
        if ( ::GetFileSizeEx( m_handle, &size ) )
        {
            assert( size.QuadPart >= 0 );
            return size.QuadPart;
        }
        assert(false);
        return 0;
    }
#endif

private:
    HANDLE m_handle;
};


}


#else


#endif


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
