#pragma once

#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <xul/config.hpp>

#include <string>
#include <vector>
#include <assert.h>
#include <stdarg.h>

#include <stdint.h>
#include <stdio.h>

#if defined(BOOST_WINDOWS)
#if !defined(_WIN32_WCE)
#include <io.h>
#include <share.h>
#endif
#else

#include <unistd.h>

#include <fcntl.h>

inline FILE* _tfsopen(const char* fname, const char* mode, int shareFlag)
{
    return fopen(fname, mode);
}

inline FILE* _fsopen(const char* fname, const char* mode, int shareFlag)
{
    return fopen(fname, mode);
}

#define _SH_DENYRW      0x10    /* deny read/write mode */
#define _SH_DENYWR      0x20    /* deny write mode */
#define _SH_DENYRD      0x30    /* deny read mode */
#define _SH_DENYNO      0x40    /* deny none mode */
#define _SH_SECURE      0x80    /* secure mode */

#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif


namespace xul {



class seek_origin
{
public:
    enum
    {
        begin = SEEK_SET,
        current = SEEK_CUR,
        end = SEEK_END,
    };
};

/// wrapper for stdio file i/o routines
class stdfile : private boost::noncopyable
{
public:
    typedef FILE* native_type;

    stdfile() : m_handle(NULL)
    {
    }
    ~stdfile()
    {
        close();
    }

#if !defined(_WIN32_WCE)
    int native_descriptor() const
    {
        if (is_open())
        {
            return fileno(m_handle);
        }
        return -1;
    }
#else
    HANDLE native_descriptor() const
    {
        if (is_open())
        {
            return fileno(m_handle);
        }
        return INVALID_HANDLE_VALUE;
    }
#endif

    /// get native file handle
    FILE* native_handle() const { return m_handle; }



#if defined(BOOST_WINDOWS) || defined(__FreeBSD__) || defined(ANDROID) || defined(XUL_MACH)
    int64_t get_position() const
    {
        fpos_t pos = 0;
        if ( 0 == fgetpos( m_handle, &pos ) )
            return pos;
        assert(false);
        return 0;
    }
    bool set_position(int64_t offset)
    {
        fpos_t pos = offset;
        if ( 0 == fsetpos( m_handle, &pos ) )
            return true;
        assert(false);
        return false;
    }
#if defined(__FreeBSD__) || defined(ANDROID) || defined(XUL_MACH)
    bool seek(int64_t offset, int origin = SEEK_SET)
    {
        return 0 == fseeko(m_handle, offset, origin);
    }
#else
    bool seek(int64_t offset, int origin = SEEK_SET)
    {
        return 0 == _fseeki64(m_handle, offset, origin);
    }
#endif
#else
    int64_t get_position() const
    {
        fpos_t pos = { 0 };
        if ( 0 == fgetpos( m_handle, &pos ) )
            return pos.__pos;
        assert(false);
        return 0;
    }
    bool set_position(int64_t offset)
    {
        fpos_t pos = { offset };
        if ( 0 == fsetpos( m_handle, &pos ) )
            return true;
        assert(false);
        return false;
    }
    bool seek(int64_t offset, int origin = SEEK_SET)
    {
        return 0 == fseeko64(m_handle, offset, origin);
    }
#endif

    long get_position32() const
    {
        return ftell(m_handle);
    }
    bool set_position32(long offset)
    {
        return seek(offset, SEEK_SET);
    }
    bool seek32(long offset, int origin = SEEK_SET)
    {
        return 0 == fseek(m_handle, offset, origin);
    }



#if defined(BOOST_WINDOWS) && !defined(_WIN32_WCE)
    bool change_size(long size)
    {
        if (false == is_open())
            return false;
        return 0 == chsize(native_descriptor(), size);
    }

    int64_t get_size() const
    {
        if (false == is_open())
            return -1;
        return _filelengthi64(native_descriptor());
    }
    int get_size32() const
    {
        if (false == is_open())
            return -1;
        return filelength(native_descriptor());
    }

#else
    /// posix version
    int64_t get_size() const
    {
        if (false == is_open())
            return -1;
        int64_t pos = get_position();
        stdfile* pThis = const_cast<stdfile*>(this);
        pThis->seek(0, SEEK_END);
        int64_t len = get_position();
        pThis->seek(pos, SEEK_SET);
        return len;
    }
    int get_size32() const
    {
        return static_cast<int>(get_size());
    }
    bool change_size(long size)
    {
        assert(false);
        return false;
    }
#endif

    bool is_open() const { return m_handle != NULL; }

    virtual void close()
    {
        if (is_open())
        {
            fclose(m_handle);
            m_handle = NULL;
        }
    }

    bool is_eof() const
    {
        return 0 != feof(m_handle);
    }

#if !defined(_WIN32_WCE)
    bool open_descriptor( int fd, const char* mode = "r" )
    {
        assert(!is_open());
        this->close();
        m_handle = fdopen( fd, mode );
        if (is_open())
            return true;
        //UTIL_DEBUG("fdopen failed " << " with " << fd << " for mode " << mode);
        return false;
    }
#endif

    bool open(const char* path, const char* mode = "r", int shareFlag = _SH_DENYNO)
    {
        assert(!is_open());
        this->close();
        m_handle = _fsopen(path, mode, shareFlag);
        if (is_open())
            return true;
        //UTIL_DEBUG("fopen failed " << " with " << path << " for mode " << mode);
        return false;
    }

#if defined(BOOST_WINDOWS) && !defined(_WIN32_WCE)
    bool open(const wchar_t* path, const wchar_t* mode = L"r", int shareFlag = _SH_DENYNO)
    {
        assert(!is_open());
        this->close();
        m_handle = _wfsopen(path, mode, shareFlag);
        if (is_open())
            return true;
        //UTIL_DEBUG("fopen failed " << " with " << path << " for mode " << mode);
        return false;
    }
#endif

    bool open_binary_read(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "rb", shareFlag);
    }

    bool open_binary_write(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "wb", shareFlag);
    }

    bool open_empty_binary_readwrite(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "w+b", shareFlag);
    }

    bool open_existing_binary_readwrite(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "r+b", shareFlag);
    }

    int read_byte()
    {
        return fgetc(m_handle);
    }

    int write_byte( int ch )
    {
        return fputc(ch, m_handle);
    }

    size_t read(void* buf, size_t size)
    {
        assert(buf != NULL && size > 0);
        assert(is_open());
        //if (!is_open())
        //{
        //    return 0;
        //}
        return fread(buf, 1, size, m_handle);
    }

    bool read_line(char* buf, int size)
    {
        assert(buf != NULL && size > 0);
        assert(is_open());
        //if (!is_open())
        //    return false;
        if (fgets(buf, size, m_handle) != NULL)
            return true;
        if (failed())
        {
            //UTIL_DEBUG("fgets failed ");
        }
        return false;
    }

    bool read_line(std::string& line)
    {
        assert(is_open());
        //if (!is_open())
        //    return false;
        const size_t max_size = 1024 * 4;
        char str[max_size + 1] = { 0 };
        if (fgets(str, max_size, m_handle) == NULL)
        {
            if (failed())
            {
                //UTIL_DEBUG("fgets failed");
            }
            return false;
        }
        line = str;
        return true;
    }


#if defined(BOOST_WINDOWS)
    bool read_line(wchar_t* buf, int size)
    {
        assert(buf != NULL && size > 0);
        assert(is_open());
        //if (!is_open())
        //    return false;
        if (fgetws(buf, size, m_handle) != NULL)
            return true;
        if (failed())
        {
            //UTIL_DEBUG("fgets failed ");
        }
        return false;
    }

    bool read_line(std::wstring& line)
    {
        assert(is_open());
        //if (!is_open())
        //    return false;
        const size_t max_size = 1024;
        wchar_t str[max_size + 1] = { 0 };
        if (fgetws(str, max_size, m_handle) == NULL)
        {
            if (failed())
            {
                //UTIL_DEBUG("fgets failed");
            }
            return false;
        }
        line = str;
        return true;
    }
#endif

    size_t write(const void* data, size_t size)
    {
        assert(data != NULL && size > 0);
        assert(is_open());
        //if (!is_open())
        //{
        //    return 0;
        //}
        return fwrite(data, 1, size, m_handle);
    }

    int write(const char* str)
    {
        assert(str != NULL);
        assert(is_open());
        //if (!is_open())
        //{
        //    return false;
        //}
        return fputs(str, m_handle);
    }
    int write(const std::string& str)
    {
        return write(str.data(), str.size());
    }

    /// argument is va_list
    int write_variant(const char* format, va_list argptr)
    {
        assert(format != NULL);
        assert(is_open());
        //if (!is_open())
        //{
        //    return 0;
        //}
        return vfprintf(m_handle, format, argptr);
    }

    /// variable number of arguments
    int write_format(const char* format, ...)
    {
        assert(format != NULL);
        va_list(args);
        va_start(args, format);
        int count = write_variant(format, args);
        va_end(args);
        return count;
    }

    /// write structure data
    template <typename StructT>
    bool write_struct(const StructT& buffer)
    {
        size_t size = write(&buffer, sizeof(StructT));
        return size == sizeof(StructT);
    }

    /// read structure data
    template <typename StructT>
    bool read_struct(StructT& buffer)
    {
        size_t size = read(&buffer, sizeof(StructT));
        return size == sizeof(StructT);
    }

    bool flush()
    {
        if (!is_open())
        {
            return false;
        }
        return EOF != fflush(m_handle);
    }

    bool failed()
    {
        assert(is_open());
        return ferror(m_handle) != 0;
    }

private:
    FILE* m_handle;
};


class stdfile_reader : public stdfile
{
public:
    bool open_binary(const char* path, int shareFlag = _SH_DENYNO)
    {
        return this->open(path, "rb", shareFlag);
    }

    bool open_text(const char* path, int shareFlag = _SH_DENYNO)
    {
        return this->open(path, "r", shareFlag);
    }

#if defined(BOOST_WINDOWS) && !defined(_WIN32_WCE)
    bool open_binary(const wchar_t* path, int shareFlag = _SH_DENYNO)
    {
        return this->open(path, L"rb", shareFlag);
    }
    bool open_text(const wchar_t* path, int shareFlag = _SH_DENYNO)
    {
        return this->open(path, L"r", shareFlag);
    }
#endif


    static bool read_binary(const char* path, size_t maxSize, std::vector<uint8_t>& buf)
    {
        assert(maxSize < 10 * 1024 * 1024);
        stdfile_reader file;
        if (!file.open_binary(path))
            return false;
        int64_t filesize = file.get_size();
        if (filesize > maxSize)
            return false;
        buf.resize(filesize);
        if (filesize <= 0)
            return true;
        if (file.read(&buf[0], filesize) != filesize)
            return false;
        return true;
    }
    static bool read_binary(const char* path, size_t maxSize, std::string& buf)
    {
        assert(maxSize < 10 * 1024 * 1024);
        stdfile_reader file;
        if (!file.open_binary(path))
            return false;
        int64_t filesize = file.get_size();
        if (filesize > maxSize)
            return false;
        buf.resize(filesize);
        if (filesize <= 0)
            return true;
        if (file.read(&buf[0], filesize) != filesize)
            return false;
        return true;
    }
};


class stdfile_writer : public stdfile
{
public:
    stdfile_writer() { }
    ~stdfile_writer()
    {
        close();
    }

    virtual void close()
    {
        if (is_open())
        {
            flush();
            stdfile::close();
        }
    }

    bool open_binary(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "wb", shareFlag);
    }

    bool open_text(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "w", shareFlag);
    }

    bool open_binary_appending(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "ab", shareFlag);
    }

    bool open_text_appending(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "a", shareFlag);
    }

    /// binary/writing mode, without truncating file
    bool open_binary_writing(const char* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, "rb+", shareFlag);
    }

    static bool write_binary(const char* path, const void* data, size_t size)
    {
        stdfile_writer file;
        if (!file.open_binary(path, _SH_DENYRW))
            return false;
        if (file.write(data, size) != size)
            return false;
        file.flush();
        return true;
    }

#if defined(BOOST_WINDOWS) && !defined(_WIN32_WCE)
    bool open_binary(const wchar_t* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, L"wb", shareFlag);
    }
    bool open_text(const wchar_t* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, L"w", shareFlag);
    }
    bool open_binary_appending(const wchar_t* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, L"ab", shareFlag);
    }
    bool open_text_appending(const wchar_t* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, L"a", shareFlag);
    }
    bool open_binary_writing(const wchar_t* path, int shareFlag = _SH_DENYWR)
    {
        return this->open(path, L"rb+", shareFlag);
    }
    static bool write_binary(const wchar_t* path, const void* data, size_t size)
    {
        stdfile_writer file;
        if (!file.open_binary(path, _SH_DENYRW))
            return false;
        if (file.write(data, size) != size)
            return false;
        file.flush();
        return true;
    }
#endif

};


}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
