#pragma once

#include <xul/config.hpp>

#include <xul/macro/fill_zero.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef XUL_WINDOWS
#include <unistd.h>
#endif


namespace xul {


class file_status
{
public:
#if defined(XUL_WINDOWS)
    typedef struct _stat32 stat_struct;
    static int get_stat( const char* filename, stat_struct* data )
    {
        return _stat32( filename, data );
    }
#else
    typedef struct stat stat_struct;
    static int get_stat( const char* filename, stat_struct* data )
    {
        return stat( filename, data );
    }
#endif

    file_status()
    {
        XUL_FILL_ZERO(m_data);
    }

    bool retrieve(const char* filename)
    {
        //assert(false == paths::has_trailing_separator(filename));
        int res = get_stat(filename, &m_data);
        return ( 0 == res );
    }
    bool retrieve(const std::string& filename)
    {
        assert(false == paths::has_trailing_separator(filename));
        return this->retrieve(filename.c_str());
    }

#if defined(XUL_WINDOWS)
    bool retrieve(const wchar_t* filename)
    {
        assert(false == wpaths::has_trailing_separator(filename));
        int res = _wstat32( filename, &m_data );
        return ( 0 == res );
    }
    bool retrieve(const std::wstring& filename)
    {
        assert(false == wpaths::has_trailing_separator(filename));
        return this->retrieve(filename.c_str());
    }
#endif

    const stat_struct& data() const { return m_data; }
    stat_struct& data() { return m_data; }

    const stat_struct* operator->() const { return &m_data; }
    stat_struct* operator->() { return &m_data; }

    bool is_regular() const
    {
        return check_mode(S_IFREG);
    }

    bool is_file() const
    {
        return is_regular();
    }

    bool is_directory() const
    {
        return check_mode(S_IFDIR);
    }

    time_t creation_time() const { return m_data.st_ctime; }
    time_t last_modify_time() const { return m_data.st_mtime; }
    time_t last_access_time() const { return m_data.st_atime; }
    int64_t file_size() const { return m_data.st_size; }


    bool check_mode(unsigned short modeFlag) const
    {
        return (m_data.st_mode & modeFlag) != 0;
    }


private:
    stat_struct m_data;
};


}
