#pragma once

/**
 * @file
 * @brief ini file
 */


#include <xul/mswin/windows.hpp>
#include <xul/std/tstring.hpp>
#include <assert.h>


namespace xul {


class ini_file
{
public:
    ini_file()
    {
    }

    void set_filename(const tstring& filename)
    {
        m_filename = filename;
    }

    void set_section(const tstring& section)
    {
        m_section = section;
    }

    const tstring& get_filename() const { return m_filename; }
    const tstring& get_section() const { return m_section; }

    DWORD get_string(LPCTSTR key, LPTSTR buffer, DWORD size, LPCTSTR defaultValue)
    {
        check_state();
        return ::GetPrivateProfileString(get_section_str(), key, defaultValue, buffer, size, get_filename_str());
    }
    tstring get_string(LPCTSTR key, LPCTSTR defaultValue)
    {
        const int max_size = 511;
        TCHAR buf[max_size + 1];
        buf[max_size] = '\0';
        DWORD length = get_string(key, buf, max_size, defaultValue);
        assert(length <= max_size);
        return tstring(buf, length);
    }
    bool set_string(LPCTSTR key, LPCTSTR value)
    {
        check_state();
        return FALSE != ::WritePrivateProfileString(get_section_str(), key, value, get_filename_str());
    }
    bool set_string(LPCTSTR key, const tstring& value)
    {
        return set_string(key, value.c_str());
    }

    UINT get_int(LPCTSTR key, int defaultValue)
    {
        check_state();
        return ::GetPrivateProfileInt(get_section_str(), key, defaultValue, get_filename_str());
    }
    bool set_int(LPCTSTR key, int value)
    {
        TCHAR str[64] = { 0 };
        ::wsprintf(str, _T("%d"), value);
        return set_string(key, str);
    }
    bool set_uint(LPCTSTR key, unsigned int value)
    {
        TCHAR str[64] = { 0 };
        ::wsprintf(str, _T("%u"), value);
        return set_string(key, str);
    }

    bool get_bool(LPCTSTR key, bool defaultValue)
    {
        return 0 != get_int(key, defaultValue);
    }
    bool set_bool(LPCTSTR key, bool value)
    {
        return set_int(key, value);
    }
    BOOL get_BOOL(LPCTSTR key, BOOL defaultValue)
    {
        UINT res = get_int(key, defaultValue);
        return (0 != res) ? TRUE : FALSE;
    }
    bool set_BOOL(LPCTSTR key, BOOL value)
    {
        return set_int(key, value);
    }

protected:
    LPCTSTR get_filename_str() const
    {
        return m_filename.c_str();
    }
    LPCTSTR get_section_str() const
    {
        return m_section.c_str();
    }
    void check_state()
    {
        assert(false == m_filename.empty());
        assert(false == m_section.empty());
    }

private:
    tstring m_filename;
    tstring m_section;
};

}
