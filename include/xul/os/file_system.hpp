#pragma once

#include <xul/config.hpp>

#include <xul/macro/fill_zero.hpp>
#include <xul/data/tchar.hpp>
#include <xul/std/tstring.hpp>
#include <xul/os/file.hpp>
#include <xul/os/paths.hpp>
#include <xul/io/stdfile.hpp>
#include <xul/data/buffer.hpp>
#include <xul/os/file_status.hpp>
#include <xul/os/file_enumerator.hpp>

#ifndef _WIN32_WCE
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <boost/algorithm/string/replace.hpp>
#include <boost/static_assert.hpp>



#if defined(XUL_WINDOWS)

#include <direct.h>

#include <shlobj.h>
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

namespace xul {
    namespace detail {

inline bool api_copy_file( const char* srcFilePath, const char* destFilePath, bool overwrite = false )
{
    BOOL fail_if_exists = overwrite ? FALSE : TRUE;
    BOOL success = ::CopyFile(srcFilePath, destFilePath, fail_if_exists);
    assert(success);
    return FALSE != success;
}

inline bool api_create_directory( const char* dir )
{
    bool success = (FALSE != ::CreateDirectory( dir, NULL ));
    assert( success );
    return success;
}

inline tstring api_get_temp_dir()
{
    const size_t max_path_size = 1023;
    char buf[max_path_size + 1];
    buf[max_path_size] = '\0';
    DWORD len = ::GetTempPath( max_path_size, buf );
    return tstring( buf, len );
}

} }


#else

#if defined(BOOST_HAS_UNISTD_H)
#include <unistd.h>
#endif

//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/convenience.hpp>
#include <boost/scoped_array.hpp>
#include <boost/version.hpp>
#include <errno.h>

namespace xul {
    namespace detail {


inline bool api_copy_file( const char* srcFilePath, const char* destFilePath, bool overwrite = false )
{
    const std::size_t buf_sz = 32768;
    boost::scoped_array<char> buf(new char [buf_sz]);
    int infile=-1, outfile=-1;  // -1 means not open

    // bug fixed: code previously did a stat()on the from_file first, but that
    // introduced a gratuitous race condition; the stat()is now done after the open()

    if ((infile = ::open(srcFilePath, O_RDONLY))< 0)
    { return false; }

    struct stat from_stat;
    if (::stat(srcFilePath, &from_stat)!= 0)
    {
        ::close(infile);
        return false;
    }

    int oflag = O_CREAT | O_WRONLY;
    if (!overwrite) oflag |= O_EXCL;
    if ((outfile = ::open(destFilePath, oflag, from_stat.st_mode))< 0)
    {
        int open_errno = errno;
        BOOST_ASSERT(infile >= 0);
        ::close(infile);
        errno = open_errno;
        return false;
    }

    ssize_t sz, sz_read=1, sz_write;
    while (sz_read > 0
        && (sz_read = ::read(infile, buf.get(), buf_sz))> 0)
    {
        // Allow for partial writes - see Advanced Unix Programming (2nd Ed.),
        // Marc Rochkind, Addison-Wesley, 2004, page 94
        sz_write = 0;
        do
        {
            if ((sz = ::write(outfile, buf.get() + sz_write,
                sz_read - sz_write))< 0)
            {
                sz_read = sz; // cause read loop termination
                break;        //  and error to be thrown after closes
            }
            sz_write += sz;
        } while (sz_write < sz_read);
    }

    if (::close(infile)< 0)sz_read = -1;
    if (::close(outfile)< 0)sz_read = -1;

    return sz_read >= 0;
}

inline bool api_create_directory( const char* dir )
{
    bool success = (0 == mkdir( dir, 0770 ));
    assert( success );
    return success;
}

#define _trename rename
#define _tunlink unlink
#define _trmdir rmdir

inline std::string api_get_temp_dir()
{
    return "/tmp";
}

} }

#endif

namespace xul {


class file_system
{
public:
    static std::string current_directory()
    {
        char buf[256 * 10] = { 0 };
        if (NULL == getcwd(buf, 256 * 10 - 1))
            return std::string();
        return std::string(buf);
    }
    static bool exists( const char* path )
    {
        assert(false == paths::has_trailing_separator(path));
        file_status s;
        return s.retrieve(path);
    }

    static bool directory_exists( const char* path )
    {
        //assert(false == paths::has_trailing_separator(path));
        if (paths::has_trailing_separator(path))
        {
            tstring new_path = paths::remove_trailing_separator(path);
            if (new_path.empty())
                return true; // new_path is empty means root directory
            file_status s;
            return s.retrieve(new_path.c_str()) && s.is_directory();
        }
        file_status s;
        return s.retrieve(path) && s.is_directory();
    }

    static bool file_exists( const char* path )
    {
        assert(false == paths::has_trailing_separator(path));
        file_status s;
        return s.retrieve(path) && s.is_file();
    }

    static bool ensure_directory_exists( const char* dir )
    {
        if ( directory_exists( dir ) )
            return true;
        return create_directories( dir );
    }

    static bool create_directory( const char* path )
    {
        assert(false == paths::has_trailing_separator(path));
        return detail::api_create_directory( path );
    }


    static bool create_directories( const char* path )
    {
        assert(false == paths::has_trailing_separator(path));
        return api_create_directories( path );
    }


    static bool copy_file( const char* srcFilePath, const char* destFilePath, bool overwrite )
    {
        return detail::api_copy_file(srcFilePath, destFilePath, overwrite);
    }

    static bool move( const char* srcFilePath, const char* destFilePath )
    {
        return 0 == ::_trename(srcFilePath, destFilePath);
    }

    static bool rename( const char* srcFilePath, const char* destFilePath )
    {
        return 0 == ::_trename(srcFilePath, destFilePath);
    }

    static bool remove_file( const char* path )
    {
        return (0 == ::_tunlink(path));
    }

    static bool remove_empty_directory( const char* path )
    {
        return ( 0 == ::_trmdir( path ) );
    }

    static tstring get_temp_directory()
    {
        return detail::api_get_temp_dir();
    }

#if defined(XUL_WINDOWS)
    static bool remove_all( const char* path )
    {
        SHFILEOPSTRUCT ops;
        XUL_FILL_ZERO( ops );
        ops.wFunc = FO_DELETE;
        ops.pFrom = path;
        ops.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
        return 0 == ::SHFileOperation( &ops );
    }

    static bool create_file( const char* path, int64_t fileSize )
    {
        xul::file f;
        if (false == f.open_write(path))
            return false;
        if (false == f.seek_from_beginning(fileSize) || false == f.set_end_of_file())
            return false;
        if (false == f.flush())
            return false;
        f.close();
        return true;
    }
#else
    static bool remove_all( const char* path )
    {
        return do_remove_all( path ? path : "" );
    }

    static bool create_file( const char* path, int64_t fileSize )
    {
        const int temp_block_size = 64 * 1024;
        xul::stdfile_writer fout;
        if (false == fout.open_binary_write(path))
        {
            return false;
        }
        xul::byte_buffer buf;
        buf.resize(temp_block_size);
        int blockCount = fileSize / temp_block_size;
        int paddingSize = fileSize % temp_block_size;
        for (int i = 0; i < blockCount; ++i)
        {
            fout.write(buf.data(), temp_block_size);
        }
        if (paddingSize > 0)
        {
            fout.write(buf.data(), paddingSize);
        }
        fout.flush();
        fout.close();
        return true;
    }
#endif

private:
#if defined(XUL_WINDOWS)
    static bool do_remove_all( const std::string& path )
    {
    }
    static bool api_create_directories( const char* dir )
    {
        std::string dirstr;
        if (false == paths::is_absolute(dir))
        {
            dirstr = paths::combine(current_directory(), dir);
        }
        else
        {
            dirstr = dir;
        }
        boost::replace_all( dirstr, "/", "\\" );
        SECURITY_ATTRIBUTES sa;
        XUL_FILL_ZERO(sa);
        sa.nLength = sizeof(sa);
        return 0 == ::SHCreateDirectoryEx( NULL, dirstr.c_str(), &sa );
    }
#else
    static bool do_remove_all( const std::string& path )
    {
        if (file_exists(path.c_str()))
            return ::remove(path.c_str()) != 0;
        if (directory_exists(path.c_str()))
        {
            file_enumerator fileenum;
            if (fileenum.open_directory(path))
            {
                for ( ; fileenum.has_item(); fileenum.move_next() )
                {
                    file_enumerator::file_item fi = fileenum.get_item();
                    do_remove_all(fi.filepath);
                }
            }
            return ::remove(path.c_str()) != 0;
        }
        assert(false);
        return false;
    }
    static bool do_create_directories( const std::string& dir )
    {
        assert(!dir.empty());
        if (directory_exists(dir.c_str()))
            return true;
        std::string parentdir = paths::get_directory(dir);
        if (!parentdir.empty() && !do_create_directories(parentdir))
            return false;
        return mkdir(dir.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    }
    static bool api_create_directories( const char* dir )
    {
        return do_create_directories(dir ? dir : "");
    }
#endif
};

}
