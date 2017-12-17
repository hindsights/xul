#pragma once

#include <xul/macro/fill_zero.hpp>

#include <boost/noncopyable.hpp>

#include <winbase.h>

namespace xul {

class file_finder : private boost::noncopyable
{
public:
    file_finder() : m_handle(INVALID_HANDLE_VALUE), m_has_item(false)
    {
        XUL_FILL_ZERO( m_item );
    }
    explicit file_finder(LPCTSTR filename) : m_handle(INVALID_HANDLE_VALUE), m_has_item(false)
    {
        XUL_FILL_ZERO( m_item );
        this->find(filename);
    }
    ~file_finder()
    {
        close();
    }

    void close()
    {
        if (is_open())
        {
            ::FindClose(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            m_has_item = false;
        }
    }
    bool is_open() const
    {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    bool find(LPCTSTR filename)
    {
        close();
        m_handle = ::FindFirstFile(filename, &m_item);
        if (!is_open())
            return false;
        m_has_item = true;
        return true;
    }

    bool has_item() const
    {
        return m_has_item;
    }
    void move_next()
    {
        assert(has_item());
        m_has_item = (FALSE != ::FindNextFile(m_handle, &m_item));
    }
    const WIN32_FIND_DATA& current() const
    {
        assert( m_has_item );
        return m_item;
    }

    bool is_directory() const
    {
        return this->match( FILE_ATTRIBUTE_DIRECTORY );
    }
    bool is_normal() const
    {
        return this->match( FILE_ATTRIBUTE_NORMAL );
    }

    bool match( DWORD attr ) const
    {
        return !!( this->current().dwFileAttributes & attr );
    }

    ULONGLONG get_size() const
    {
        return make_uint64( this->current().nFileSizeHigh, this->current().nFileSizeLow );
    }

    const TCHAR* get_name() const
    {
        return this->current().cFileName;
    }


    static bool exists( const TCHAR* path )
    {
        file_finder finder( path );
        return finder.has_item();
    }

    static bool directory_exists( const TCHAR* path )
    {
        file_finder finder( path );
        if ( false == finder.has_item() )
            return false;
        DWORD attr = finder.current().dwFileAttributes;
        return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    static bool file_exists( const TCHAR* path )
    {
        file_finder finder( path );
        if ( false == finder.has_item() )
            return false;
        DWORD attr = finder.current().dwFileAttributes;
        return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

protected:
    static ULONGLONG make_uint64( DWORD high, DWORD low )
    {
        ULARGE_INTEGER val;
        val.HighPart = high;
        val.LowPart = low;
        return val.QuadPart;
    }


private:
    HANDLE    m_handle;
    WIN32_FIND_DATA    m_item;
    bool    m_has_item;
};

}

