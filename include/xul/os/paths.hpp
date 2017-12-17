#pragma once

/**
 * @file
 * @brief path functions
 */

#include <boost/config.hpp>

#include <xul/std/strings.hpp>
#include <xul/std/tstring.hpp>
#include <xul/data/tchar.hpp>

#if defined(XUL_RUN_TEST)
#include <xul/util/test_case.hpp>
#endif

#include <boost/noncopyable.hpp>

#include <utility>
#include <string>
#include <sstream>
#include <locale>
#include <assert.h>


namespace xul {


const char XUL_PATH_BACKSLASH = '\\';
const char XUL_PATH_SLASH = '/';


template <typename StringT>
class windows_path_traits
{
public:
    typedef StringT string_type;
    typedef typename StringT::value_type char_type;
    typedef typename StringT::traits_type char_traits_type;

    static char_type colon()
    {
        return char_traits_type::to_char_type(':');
    }

    static char_type slash()
    {
        return char_traits_type::to_char_type(XUL_PATH_SLASH);
    }

    static char_type backslash()
    {
        return char_traits_type::to_char_type(XUL_PATH_BACKSLASH);
    }

    static char_type separator()
    {
        return backslash();
    }
    static char_type extension_separator()
    {
        return char_traits_type::to_char_type('.');
    }

    static bool is_separator(char_type ch)
    {
        return slash() == ch || backslash() == ch;
    }

    static bool is_absolute(const string_type& path)
    {
        if ( path.empty() )
            return false;
        if ( is_separator( path[0] ) )
        {
            return true;
        }
        if ( path.size() < 2 )
            return false;
        if ( ( colon() == path[1] ) && isalpha(path[0]) )
        {
            if ( path.size() < 3 )
                return false;
            return is_separator( path[2] );
        }
        return false;
    }
};

template <typename StringT>
class posix_path_traits
{
public:
    typedef StringT string_type;
    typedef typename StringT::value_type char_type;
    typedef typename StringT::traits_type char_traits_type;

    static char_type slash()
    {
        return char_traits_type::to_char_type(XUL_PATH_SLASH);
    }

    static char_type separator()
    {
        return slash();
    }
    static char_type extension_separator()
    {
        return char_traits_type::to_char_type('.');
    }

    static bool is_separator(char_type ch)
    {
        return slash() == ch;
    }

    static bool is_absolute(const string_type& path)
    {
        return ( false == path.empty() ) && is_separator( path[0] );
    }
};


template <typename TraitsT>
class basic_paths
{
public:
    typedef TraitsT traits_type;
    typedef typename TraitsT::string_type string_type;
    typedef typename TraitsT::char_type char_type;
    typedef typename TraitsT::char_traits_type char_traits_type;

    typedef std::pair<string_type, string_type> string_pair_type;

    static string_type combine(const string_type& dir, const string_type& filename)
    {
        return join(dir, filename);
    }
    static string_type join(const string_type& dir, const string_type& filename)
    {
        if (dir.empty())
            return filename;
        string_type newDir = add_trailing_separator(dir);
        return newDir + filename;
    }

    static string_type join(const std::vector<string_type>& parts)
    {
        std::basic_ostringstream<typename traits_type::char_type> oss;
        bool lastPartHasTrailingSep = true;
        for (size_t index = 0; index < parts.size(); ++index)
        {
            if (false == lastPartHasTrailingSep)
            {
                oss << traits_type::separator();
            }
            oss << parts[index];
            lastPartHasTrailingSep = has_trailing_separator(parts[index]);
        }
        return oss.str();
    }

    static string_pair_type split_filename(const string_type& path)
    {
        if (path.empty())
        {
            return std::make_pair( string_type(), string_type() );
        }
        assert( path.size() >= 1 );
        int pos = path.size() - 1;
        for ( ;; )
        {
            if ( traits_type::is_separator( path[pos] ) )
            {
                if (0 == pos)
                {
                    return std::make_pair(path.substr(0, pos + 1), path.substr(pos + 1));
                }
                return std::make_pair(path.substr(0, pos), path.substr(pos + 1));
            }
            if ( pos <= 0 )
            {
                break;
            }
            --pos;
        }
        return std::make_pair( string_type(), path );
    }

    static string_pair_type split_extension(const string_type& path)
    {
        if (path.empty())
        {
            return string_pair_type( string_type(), string_type() );
        }
        assert( path.size() >= 1 );
        int pos = path.size() - 1;
        for ( ;; )
        {
            if ( traits_type::extension_separator() == path[pos] )
            {
                return strings::split_at( path, pos );
            }
            if ( traits_type::is_separator( path[pos] ) )
            {
                break;
            }
            if ( 0 == pos )
            {
                break;
            }
            --pos;
        }
        return std::make_pair( path, string_type() );
    }

    static void split(const string_type& path, std::vector<string_type>& parts, bool ignore_empty = false)
    {
        parts.clear();
        int startPos = 0;
        for (size_t pos = 0; pos < path.size(); ++pos)
        {
            if ( traits_type::is_separator( path[pos] ) )
            {
                std::string part;
                if (0 == startPos)
                {
                    part = path.substr(startPos, pos + 1 - startPos);
                }
                else
                {
                    part = path.substr(startPos, pos - startPos);
                }
                if (!ignore_empty || !part.empty())
                    parts.push_back(part);
                startPos = pos + 1;
            }
        }
        parts.push_back(path.substr(startPos));
    }

    static string_type get_directory(const string_type& path)
    {
        return remove_trailing_separator( split_filename(path).first );
    }

    static string_type remove_filename(const string_type& path)
    {
        return split_filename(path).first;
    }

    static string_type get_filename(const string_type& path)
    {
        return split_filename(path).second;
    }

    static string_type remove_extension(const string_type& path)
    {
        return split_extension(path).first;
    }

    static string_type get_extension(const string_type& path)
    {
        string_pair_type res = split_extension(path);
        return res.second;
    }

    static string_type get_basename(const string_type& path)
    {
        return remove_extension(get_filename(path));
    }


    static bool has_extension(const string_type& path)
    {
        return false == get_extension(path).empty();
    }


    static string_type replace_directory(const string_type& path, const string_type& newDir)
    {
        string_type filename = get_filename(path);
        return join(newDir, filename);
    }

    static string_type replace_filename(const string_type& path, const string_type& newFilename)
    {
        string_type dir = get_directory(path);
        return join(dir, newFilename);
    }

    static string_type replace_extension(const string_type& path, const string_type& newExt)
    {
        string_type pathWithoutExt = remove_extension(path);
        return pathWithoutExt + newExt;
    }

    static string_type replace_basename(const string_type& path, const string_type& newBasename)
    {
        string_pair_type dirs = split_filename(path);
        string_pair_type exts = split_extension(dirs.second);
        return join(dirs.first, newBasename + traits_type::extension_separator() + exts.second);
    }


    static bool is_absolute(const string_type& path)
    {
        return traits_type::is_absolute( path );
    }

    static bool has_trailing_separator(const string_type& path)
    {
        if (path.empty())
            return false;
        return traits_type::is_separator( path[path.size() - 1] );
    }

    static string_type add_trailing_separator(const string_type& path)
    {
        if (has_trailing_separator(path))
            return path;
        string_type res = path + traits_type::separator();
        assert( has_trailing_separator( res ) );
        return res;
    }

    static string_type remove_trailing_separator(const string_type& path)
    {
        if ( false == has_trailing_separator(path) )
            return path;
        if ( path.size() == 1 )
            return path;
        assert ( false == path.empty() );
        string_type res = path.substr(0, path.size() - 1);
        assert( false == has_trailing_separator( res ) );
        return res;
    }

};

typedef basic_paths<windows_path_traits<std::string> > windows_paths;
typedef basic_paths<windows_path_traits<std::wstring> > windows_wpaths;
typedef basic_paths<posix_path_traits<std::string> > posix_paths;
typedef basic_paths<posix_path_traits<std::wstring> > posix_wpaths;
typedef posix_paths url_paths;
typedef posix_wpaths url_wpaths;

#if defined(BOOST_WINDOWS)
typedef windows_path_traits<std::string> default_path_traits;
typedef windows_path_traits<std::wstring> default_wpath_traits;
#else
typedef posix_path_traits<std::string> default_path_traits;
typedef posix_path_traits<std::wstring> default_wpath_traits;
#endif

typedef basic_paths<default_path_traits> paths;
typedef basic_paths<default_wpath_traits> wpaths;

#ifdef XUL_RUN_TEST

class paths_test_case : public xul::test_case
{
public:
    static std::pair<std::string, std::string> make_string_pair(const std::string& x, const std::string& y)
    {
        return std::make_pair(x, y);
    }
    virtual void do_run()
    {
        assert(paths::split_filename("") == make_string_pair("", ""));
        assert(paths::split_filename("/") == make_string_pair("/", ""));
        assert(paths::split_filename("abc/c.txt") == make_string_pair("abc", "c.txt"));
        assert(paths::split_filename("abc/") == make_string_pair("abc", ""));
        assert(paths::split_extension("abc/c.txt") == make_string_pair("abc/c", ".txt"));
        assert(paths::get_basename("abc/cde.txt") == "cde");
        assert(paths::get_directory("abc/cde.txt") == "abc");
        assert(paths::get_filename("abc/cde.txt") == "cde.txt");
        assert(windows_paths::get_directory("abc\\cde.txt") == "abc");
        assert(windows_paths::get_basename("abc\\cde.txt\\") == "");
        assert(windows_paths::get_directory("abc\\cde.txt\\") == "abc\\cde.txt");
        assert(windows_paths::get_filename("abc\\cde.txt\\") == "");
        assert(paths::replace_extension("abc.txt", "") == "abc");
    }
};

#endif


}
