#pragma once

/**
 * @file
 * @brief bmf(binary message format)
 */

#include <xul/io/bmf_format.hpp>
#include <xul/io/bmf_integer_encoding.hpp>
#include <xul/io/input_stream.hpp>
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include <vector>
#include <list>
#include <map>
#include <stddef.h>
#include <stdint.h>


namespace xul {


class bmf_reader : boost::noncopyable, public bmf_format
{
public:
    typedef std::list<boost::any> list_type;
    typedef std::map<std::string, boost::any> dict_type;

    explicit bmf_reader(input_stream& is) : m_is(is)
    {
    }
    virtual ~bmf_reader()
    {
    }

    boost::any read()
    {
        if (false == m_is.is_available(1))
            return boost::any();
        uint8_t tag = m_is.read_byte();
        return read_item(tag);
    }
    static int64_t extract_integer(const boost::any& item, int64_t default_val)
    {
        if (item.empty())
            return default_val;
        return boost::any_cast<int64_t>(item);
    }
    static int64_t extract_dict_integer(const dict_type& dict, const std::string& key, int64_t default_val)
    {
        return extract_integer(xul::maps::get(dict, key, boost::any()), default_val);
    }

    static std::string extract_string(const boost::any& item, const std::string& default_val = std::string())
    {
        if (item.empty())
            return default_val;
        return boost::any_cast<std::string>(item);
    }
    static std::string extract_dict_string(const dict_type& dict, const std::string& key, std::string default_val = std::string())
    {
        return extract_string(xul::maps::get(dict, key, boost::any()), default_val);
    }

private:
    boost::any read_item(int tag)
    {
        switch (tag)
        {
        case integer_tag:
            return read_integer();
        case string_tag:
            return read_string();
        case list_tag:
            return read_list();
        case dict_tag:
            return read_dict();
        }
        return boost::any();
    }
    boost::any read_integer()
    {
        int64_t val = 0;
        if (false == bmf_integer_encoding::decode(m_is, &val))
            return boost::any();
        return boost::any(val);
    }
    boost::any read_string()
    {
        std::string s;
        if (false == try_read_string(s))
            return boost::any();
        return boost::any(s);
    }
    bool try_read_string(std::string& s)
    {
        int64_t val = 0;
        if (false == bmf_integer_encoding::decode(m_is, &val))
            return false;
        if (val > 128 * 1024)
            return false;
        s.resize(static_cast<size_t>(val));
        if (false == m_is.read_n(reinterpret_cast<uint8_t*>(&s[0]), static_cast<size_t>(val)))
            return false;
        return true;
    }
    boost::any read_list()
    {
        list_type items;
        for (;;)
        {
            if (false == m_is.is_available(1))
                return boost::any();
            uint8_t tag = m_is.read_byte();
            if (end_tag == tag)
                break;
            items.push_back(read_item(tag));
        }
        return boost::any(items);
    }
    boost::any read_dict()
    {
        dict_type items;
        for (;;)
        {
            if (false == m_is.is_available(1))
                return boost::any();
            uint8_t tag = m_is.read_byte();
            if (end_tag == tag)
                break;
            if (tag != string_tag)
                return boost::any();
            std::string key;
            if (false == try_read_string(key))
                return boost::any();
            items.insert(std::make_pair(key, read()));
        }
        return boost::any(items);
    }

private:
    input_stream& m_is;
};


}
