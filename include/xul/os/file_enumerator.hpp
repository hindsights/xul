#pragma once

#include <xul/config.hpp>
#include <boost/noncopyable.hpp>
#include <xul/data/numerics.hpp>
#include <xul/os/paths.hpp>
#include <stdint.h>

#if defined(XUL_WINDOWS)

#include <xul/mswin/file_finder.hpp>

#else

#include <xul/os/file_status.hpp>
#include <sys/types.h>
#include <dirent.h>

#endif


namespace xul {


#if defined(XUL_WINDOWS)

class file_enumerator : private boost::noncopyable
{
public:
    class file_item
    {
    public:
        const WIN32_FIND_DATA& data;

        explicit file_item(const WIN32_FIND_DATA& findData) : data(findData)
        {
        }

        const char* get_filename() const { return data.cFileName; }
        uint64_t get_size() const { return xul::numerics::make_qword(data.nFileSizeHigh, data.nFileSizeLow); }

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
            return !!( data.dwFileAttributes & attr );
        }

    };

    void close()
    {
        m_finder.close();
    }
    bool is_open() const
    {
        return m_finder.is_open();
    }

    bool enumerate(const char* filename)
    {
        return m_finder.find(filename);
    }

    bool open_directory(const std::string& dir)
    {
        return enumerate(xul::paths::combine(dir, "*.*").c_str());
    }

    bool has_item() const
    {
        return m_finder.has_item();
    }

    file_item get_item() const
    {
        return file_item(m_finder.current());
    }
    void move_next()
    {
        m_finder.move_next();
    }

private:
    xul::file_finder m_finder;
};

#else

class file_enumerator : private boost::noncopyable
{
public:
    class file_item
    {
    public:
        std::string filename;
        std::string filepath;
        xul::file_status data;

        const char* get_filename() const { return filename.c_str(); }
        uint64_t get_size() const { return data.file_size(); }

        bool is_directory() const
        {
            return data.is_directory();
        }
        bool is_normal() const
        {
            return data.is_regular();
        }

        bool match( uint16_t attr ) const
        {
            return data.check_mode(attr);
        }

    };

    file_enumerator() : m_dir(NULL), m_item(NULL)
    {

    }
    ~file_enumerator()
    {
        close();
    }

    void close()
    {
        if (m_dir)
        {
            closedir(m_dir);
            m_dir = NULL;
        }
        m_item = NULL;
    }
    bool is_open() const
    {
        return m_dir != NULL;
    }

    bool enumerate(const char* filename)
    {
        close();
        m_current_directory = filename;
        m_dir = opendir(filename);
        if (!m_dir)
        {
            //printf("failed to opendir\n");
            return false;
        }
        move_next();
        return has_item();
    }

    bool open_directory(const std::string& dir)
    {
        return enumerate(dir.c_str());
    }

    bool has_item() const
    {
        return m_item != NULL;
    }
    void move_next()
    {
        m_item = readdir(m_dir);
    }

    file_item get_item() const
    {
        file_item item;
        if (m_item)
        {
            item.filename = std::string(m_item->d_name, m_item->d_reclen);
            item.filepath = xul::paths::combine(m_current_directory, item.filename);
            item.data.retrieve(item.filepath.c_str());
        }
        return item;
    }

private:
    std::string m_current_directory;
    DIR* m_dir;
    dirent* m_item;
};

#endif

}


