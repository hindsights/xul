#pragma once

#include <xul/util/options.hpp>
#include <xul/util/data_parser.hpp>

/**
 * @file
 * @brief helper functions for options class
 */


namespace xul {


class options_wrapper
{
public:
    xul::options& options;

    explicit options_wrapper(xul::options& opts) : options(opts)
    {
    }

    void add_string(const std::string& name, std::string* item, const std::string& default_val)
    {
        do_add(new data_parser::string_item_parser(name, *item, default_val));
    }
    void add_string(const std::string& name, const std::string& default_val)
    {
        do_add(new data_parser::string_value_item_parser(name, default_val));
    }
    void add(const std::string& name, std::string* item, const std::string& default_val)
    {
        do_add(new data_parser::string_item_parser(name, *item, default_val));
    }
    void add(const std::string& name, std::string* item, const char* default_val = "")
    {
        do_add(new data_parser::string_item_parser(name, *item, default_val));
    }
    void add(const std::string& name, bool* item, bool default_val)
    {
        do_add(new data_parser::bool_parser(name, *item, default_val));
    }
    void add_switch(const std::string& name, bool* item, bool default_val)
    {
        do_add(new data_parser::switch_parser(name, *item, default_val));
    }
    template <typename EnumT>
    void add_enum(const std::string& name, typename EnumT::enum_type& item, typename EnumT::enum_type default_val)
    {
        do_add(new data_parser::enum_parser<EnumT>(name, item, default_val));
    }
    template <typename NumberT>
    void add_unit(const std::string& name, NumberT* item, const data_parser::numeric_units<NumberT>& units, NumberT default_val, const std::string& default_unit_name = std::string())
    {
        do_add(new data_parser::numeric_units_item_parser<NumberT>(name, *item, units, default_val, default_unit_name));
    }
    template <typename NumberT>
    void add_decimal_byte_count(const std::string& name, NumberT* item, NumberT default_val, const std::string& default_unit_name = std::string())
    {
        add_unit<NumberT>(name, item, data_parser::decimal_byte_count_units<NumberT>::instance(), default_val, default_unit_name);
    }
    template <typename NumberT>
    void add_binary_byte_count(const std::string& name, NumberT* item, NumberT default_val, const std::string& default_unit_name = std::string())
    {
        add_unit<NumberT>(name, item, data_parser::binary_byte_count_units<NumberT>::instance(), default_val, default_unit_name);
    }
    template <typename T>
    void add(const std::string& name, T* item, const T& default_val)
    {
        do_add(new data_parser::number_parser<T, data_type_integer>(name, *item, default_val));
    }
    template <typename T>
    void add_integer(const std::string& name, T* item, const T& default_val)
    {
        do_add(new data_parser::number_parser<T, data_type_integer>(name, *item, default_val));
    }
    template <typename T>
    void add_float(const std::string& name, T* item, const T& default_val)
    {
        do_add(new data_parser::number_parser<T, data_type_float>(name, *item, default_val));
    }
    template <typename T>
    void add_object(const std::string& name, T* item, const T& default_val)
    {
        do_add(new data_parser::general_item_parser<T, data_type_string>(name, *item, default_val));
    }

    bool parse(const std::string& name, const std::string& val)
    {
        std::string realname = strings::trim_copy(name);
        std::string realval = strings::trim_copy(val);
        return options.set(realname.c_str(), realval.c_str());
#if 0
        option* opt = options.get_option(realname.c_str());
        if (!opt)
            return false;
        return opt->parse(realval.c_str());
#endif
    }

    bool parse(const std::string& expr, char sep = '=')
    {
        std::pair<std::string, std::string> keyval = strings::split_pair(expr, sep);;
        return parse(keyval.first, keyval.second);
    }
protected:
    void do_add(option* opt)
    {
        options.add(opt);
        opt->reset();
    }
};


}
