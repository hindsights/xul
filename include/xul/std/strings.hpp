#pragma once

#include <boost/config.hpp>

#include <xul/std/tstring.hpp>
#include <xul/data/tchar.hpp>
#include <xul/log/loghelper.hpp>

#include <vector>
#include <utility>

//#include <tchar.h>
#include <assert.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <stdarg.h>

#include <iosfwd>
#include <sstream>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif


namespace xul {

//using std::ostringstream;
//using std::wostringstream;

#ifdef UNICODE

typedef std::wostream tostream;
typedef std::wistream tistream;

typedef std::wostringstream tostringstream;
typedef std::wistringstream tistringstream;

typedef std::wofstream tofstream;
typedef std::wifstream tifstream;

#else

typedef std::ostream tostream;
typedef std::istream tistream;

typedef std::ostringstream tostringstream;
typedef std::istringstream tistringstream;

typedef std::ofstream tofstream;
typedef std::ifstream tifstream;

#endif



/// utility functions for string manipulation
class strings
{
public:
    static const char* get_safe_data(const char* s)
    {
        return s ? s : "";
    }

    /// case-insensitive equals
    static bool iequals(const std::string& s1, const std::string& s2)
    {
        return (s1.size() == s2.size()) && (strncasecmp(s1.c_str(), s2.c_str(), s1.size()) == 0);
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static int icompare(const std::basic_string<CharT, TraitsT, AllocatorT>& x, const std::basic_string<CharT, TraitsT, AllocatorT>& y)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        string_type x1 = to_lower_copy(x);
        string_type y1 = to_lower_copy(y);
        if (x1 == y1)
            return 0;
        if (x1 > y1)
            return 1;
        return -1;
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static void make_upper(std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static void make_lower(std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> to_upper_copy(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        std::basic_string<CharT, TraitsT, AllocatorT> result = s;
        make_upper(result);
        return result;
    }
    template <typename CharT>
    static std::basic_string<CharT> to_upper_copy(const CharT* s)
    {
        return to_upper_copy(std::basic_string<CharT>(s));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> to_lower_copy(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        std::basic_string<CharT, TraitsT, AllocatorT> result = s;
        make_lower(result);
        return result;
    }
    template <typename CharT>
    static std::basic_string<CharT> to_lower_copy(const CharT* s)
    {
        return to_lower_copy(std::basic_string<CharT>(s));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> extract_string(const std::basic_string<CharT, TraitsT, AllocatorT>& s, size_t start, size_t end)
    {
        if (end != std::basic_string<CharT, TraitsT, AllocatorT>::npos)
            return s.substr(start, end - start);
        return s.substr(start);
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> to_upper(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        return to_upper_copy(s);
    }
    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> to_upper(const CharT* s)
    {
        return to_upper_copy(std::basic_string<CharT, TraitsT, AllocatorT>(s));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> to_lower(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        return to_lower_copy(s);
    }

    static std::string join(const std::string& sep, const std::vector<std::string>& strs)
    {
        if (strs.empty())
            return std::string();
        std::ostringstream oss;
        oss << strs[0];
        for (int i = 1; i < strs.size(); ++i)
        {
            oss << sep << strs[i];
        }
        return oss.str();
    }

    template <typename CharT, typename TraitsT, typename AllocatorT, typename OutputIteratorT>
    static void split(OutputIteratorT output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter, int maxSplit = 0)
    {
        if (src.size() <= 0)
            return;

        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t startPos = 0;
        size_t pos;
        int splitedPartCount = 0;
        for (;;)
        {
            splitedPartCount++;
            // if maxSplit is greater than 0, then it's enabled
            if (maxSplit > 0 && splitedPartCount >= maxSplit)
            {
                *output++ = src.substr(startPos);
                break;
            }
            pos = src.find(delimiter, startPos);
            if (pos == string_type::npos)
            {
                *output++ = src.substr(startPos);
                break;
            }
            *output++ = src.substr(startPos, pos - startPos);
            startPos = pos + 1;
        }
    }

    template <typename CharT, typename TraitsT, typename AllocatorT, typename VectorAllocatorT>
    static void split(std::vector<std::basic_string<CharT, TraitsT, AllocatorT>, VectorAllocatorT>& output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter, int maxSplit = 0)
    {
        split(std::back_inserter(output), src, delimiter, maxSplit);
    }

    template <typename CharT, typename TraitsT, typename AllocatorT, typename OutputIteratorT>
    static void split(OutputIteratorT output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter, int maxSplit = 0)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t startPos = 0;
        size_t pos;
        int splitedPartCount = 0;
        for (;;)
        {
            splitedPartCount++;
            if (maxSplit > 0 && splitedPartCount >= maxSplit)
            {
                *output++ = src.substr(startPos);
                break;
            }
            pos = src.find(delimiter, startPos);
            if (pos == string_type::npos)
            {
                *output++ = src.substr(startPos);
                break;
            }
            *output++ = src.substr(startPos, pos - startPos);
            startPos = pos + delimiter.size();
        }
    }

    template <typename CharT, typename TraitsT, typename AllocatorT, typename VectorAllocatorT>
    static void split_of(std::vector<std::basic_string<CharT, TraitsT, AllocatorT>, VectorAllocatorT>& output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter, int maxSplit = 0)
    {
        split_of(std::back_inserter(output), src, delimiter, maxSplit);
    }

    /// split string by find_first_of way
    template <typename CharT, typename TraitsT, typename AllocatorT, typename OutputIteratorT>
    static void split_of(OutputIteratorT output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter, int maxSplit = 0)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t startPos = 0;
        size_t pos;
        int splitedPartCount = 0;
        for (;;)
        {
            splitedPartCount++;
            if (maxSplit > 0 && splitedPartCount >= maxSplit)
            {
                *output++ = src.substr(startPos);
                break;
            }
            pos = src.find_first_of(delimiter, startPos);
            if (string_type::npos == pos)
            {
                *output++ = src.substr(startPos);
                break;
            }
            *output++ = src.substr(startPos, pos - startPos);
            startPos = pos + 1;
        }
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t pos = src.find(delimiter, 0);
        if (pos == string_type::npos)
            return std::make_pair(src, string_type());
        assert(pos + 1 <= src.size());
        return std::make_pair(src.substr(0, pos), src.substr(pos + 1));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_pair_of(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t pos = src.find_first_of(delimiter, 0);
        if (pos == string_type::npos)
            return std::make_pair(src, string_type());
        assert(pos + delimiter.size() <= src.size());
        return std::make_pair(src.substr(0, pos), src.substr(pos + delimiter.size()));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t pos = src.find(delimiter, 0);
        if (pos == string_type::npos)
            return std::make_pair(src, string_type());
        assert(pos + delimiter.size() <= src.size());
        return std::make_pair(src.substr(0, pos), src.substr(pos + delimiter.size()));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter)
    {
        return split_pair(src, delimiter, TraitsT::length(delimiter));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_pair_of(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter)
    {
        return split_pair_of(src, delimiter, TraitsT::length(delimiter));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter, size_t delimiterSize)
    {
        typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
        size_t pos = src.find(delimiter, 0, delimiterSize);
        if (pos == string_type::npos)
            return std::make_pair(src, string_type());
        assert(pos + delimiterSize <= src.size());
        return std::make_pair(src.substr(0, pos), src.substr(pos + delimiterSize));
    }


    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> >
        split_at(const std::basic_string<CharT, TraitsT, AllocatorT>& src, size_t pos)
    {
        assert(pos <= src.size());
        return std::make_pair(src.substr(0, pos), src.substr(pos));
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static void trim_spaces(std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        trim(s, " \t\r\n");
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> trim_spaces_copy(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
    {
        return trim_copy(s, " \t\r\n");
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static void trim(std::basic_string<CharT, TraitsT, AllocatorT>& s, const CharT* delimiter)
    {
        assert(TraitsT::length(delimiter) > 0);
        s.erase(0, s.find_first_not_of(delimiter));
        s.erase(s.find_last_not_of(delimiter) + 1);
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static void trim(std::basic_string<CharT, TraitsT, AllocatorT>& s, const std::string& delimiter)
    {
        assert(false == delimiter.empty());
        s.erase(0, s.find_first_not_of(delimiter));
        s.erase(s.find_last_not_of(delimiter) + 1);
    }

    template <typename AllocatorT>
    static void trim(std::basic_string<char, std::char_traits<char>, AllocatorT>& str)
    {
        trim_spaces(str);
    }

    template <typename AllocatorT>
    static void trim(std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocatorT>& str)
    {
        trim_spaces(str);
}

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static std::basic_string<CharT, TraitsT, AllocatorT> trim_copy(const std::basic_string<CharT, TraitsT, AllocatorT>& str, const CharT* delimiter)
    {
        std::basic_string<CharT, TraitsT, AllocatorT> s = str;
        trim(s, delimiter);
        return s;
    }

    template <typename AllocatorT>
    static std::basic_string<char, std::char_traits<char>, AllocatorT> trim_copy(const std::basic_string<char, std::char_traits<char>, AllocatorT>& str)
    {
        return trim_spaces_copy(str);
    }

    template <typename AllocatorT>
    static std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocatorT> trim_copy(const std::basic_string<wchar_t, std::char_traits<wchar_t>, AllocatorT>& str)
    {
        return trim_spaces_copy(str);
    }

    template <typename T>
    static std::string format_object(const T& obj)
    {
        std::ostringstream oss;
        oss << obj;
        return oss.str();
    }

    template <typename T>
    static std::string format_writable(const T& obj)
    {
        std::ostringstream oss;
        obj.write(oss);
        return oss.str();
    }


    static std::string format(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        std::string str = vformat(format, args);
        va_end(args);
        return str;
    }
    static std::string vformat(const char* format, va_list argList)
    {
        const size_t max_size = 1024 * 8 - 1;
        char str[max_size + 1];
        str[max_size] = 0;
        int count = vsnprintf(str, max_size, format, argList);
        if (count < 0)
        {
            assert(false);
            return std::string();
        }
        assert(count < max_size);
        return std::string(str, count);
    }

#ifdef BOOST_WINDOWS
    static std::wstring format(const wchar_t* format, ...)
    {
        va_list args;
        va_start(args, format);
        std::wstring str = vformat(format, args);
        va_end(args);
        return str;
    }
    static std::wstring vformat(const wchar_t* format, va_list argList)
    {
        const size_t max_size = 1024;
        wchar_t str[max_size + 1];
        str[max_size] = 0;
        int count = _vsnwprintf(str, max_size, format, argList);
        if (count < 0)
        {
            assert(false);
            return std::wstring();
        }
        assert(count < max_size);
        return std::wstring(str, count);
    }
#endif

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static bool is_upper(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
    {
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (!::isupper(str[i]))
                return false;
        }
        return true;
    }

    template <typename CharT, typename TraitsT, typename AllocatorT>
    static bool is_digit(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
    {
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (!::isdigit(str[i]))
                return false;
        }
        return true;
    }

};


class string_iequal_to : public std::binary_function<std::string, std::string, bool>
{
public:
    bool operator()(const std::string& x, const std::string& y) const
    {
        return strings::to_lower_copy(x) == strings::to_lower_copy(y);
    }
};


template <typename StringT>
struct string_iless
{
    bool operator()( const StringT& x, const StringT& y ) const
    {
        return strings::to_lower_copy(x) < strings::to_lower_copy(y);
    }
};


}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
