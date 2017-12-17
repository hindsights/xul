#pragma once

#include <xul/std/maps.hpp>
#include <map>
#include <string>


namespace xul {


class enum_metadata
{
public:
    typedef std::map<int, std::string> name_table;
    typedef std::map<std::string, int> value_table;
    enum_metadata()
    {
    }
    void add(int val, const std::string& name)
    {
        m_names[val] = name;
        m_values[name] = val;
    }
    bool try_parse(const std::string& s, int& val) const
    {
        value_table::const_iterator iter = m_values.find(s);
        if (iter == m_values.end())
            return false;
        val = iter->second;
        return true;
    }
    int parse(const std::string& s, int default_val) const
    {
        int typeval = default_val;
        return try_parse(s, typeval) ? typeval : default_val;
    }
    template <typename EnumT>
    bool try_parse(const std::string& s, EnumT& val) const
    {
        int intval = 0;
        if (false == try_parse(s, intval))
            return false;
        val = static_cast<EnumT>(intval);
        return true;
    }
    template <typename EnumT>
    EnumT parse(const std::string& s, EnumT default_val) const
    {
        return static_cast<EnumT>(parse(s, static_cast<int>(default_val)));
    }
    std::string get_name(int val) const
    {
        return maps::get(m_names, val);
    }
private:
    name_table m_names;
    value_table m_values;
};


template <typename T>
class enum_impl : public T
{
public:
    typedef T enum_container_type;;

    static const typename T::metadata_type& metadata()
    {
        static typename T::metadata_type the_metadata;
        return the_metadata;
    }
    static bool try_parse(const std::string& s, typename T::enum_type& val)
    {
        return metadata().try_parse(s, val);
    }
    static typename T::enum_type parse(const std::string& s, typename T::enum_type default_val)
    {
        return metadata().parse(s, default_val);
    }
    static bool try_parse(const std::string& s, int& val)
    {
        return metadata().try_parse(s, val);
    }
    static int parse(const std::string& s, int default_val)
    {
        return metadata().parse(s, default_val);
    }
    static std::string get_name(int val)
    {
        return metadata().get_name(val);
    }
    static std::string get_name(typename T::enum_type val)
    {
        return metadata().get_name(val);
    }
};


template <typename EnumT, typename MetadataT>
class simple_enum_impl
{
public:
    typedef EnumT enum_type;
    typedef MetadataT metadata_type;

    static const metadata_type& metadata()
    {
        static metadata_type the_metadata;
        return the_metadata;
    }
    static bool try_parse(const std::string& s, enum_type& val)
    {
        return metadata().try_parse(s, val);
    }
    static enum_type parse(const std::string& s, enum_type default_val)
    {
        return metadata().parse(s, default_val);
    }
    static bool try_parse(const std::string& s, int& val)
    {
        return metadata().try_parse(s, val);
    }
    static int parse(const std::string& s, int default_val)
    {
        return metadata().parse(s, default_val);
    }
    static std::string get_name(int val)
    {
        return metadata().get_name(val);
    }
    static std::string get_name(enum_type val)
    {
        return metadata().get_name(val);
    }
};


}
