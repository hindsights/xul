#pragma once

#include <xul/std/maps.hpp>
#include <xul/std/strings.hpp>
#include <xul/data/numerics.hpp>
#include <xul/util/singleton.hpp>
#include <xul/util/option.hpp>
#include <xul/data/printables.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/macro/foreach.hpp>
#include <xul/io/structured_writer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>
//#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <stdint.h>


namespace xul {


class data_parser
{
public:
    class data_item_parser;
    typedef boost::intrusive_ptr<data_item_parser> data_item_parser_ptr;
    typedef std::map<std::string, data_item_parser_ptr> item_parser_collection;

    class data_item_parser : public object_impl<option>
    {
    public:
        explicit data_item_parser(const std::string& name)
            : m_name(name)
            , m_public(true)
            , m_freezed(false)
            , m_modified(false)
        {
        }
        virtual ~data_item_parser() { }

        virtual bool parse(const char* s)
        {
            if (is_freezed())
                return false;
            if (!s)
                return false;
            if (!this->do_parse(std::string(s)))
                return false;
            this->set_dirty();
            return true;
        }
        virtual const char* get_name() const
        {
            return m_name.c_str();
        }
        virtual void freeze()
        {
            m_freezed = true;
        }
        virtual bool is_freezed() const
        {
            return m_freezed;
        }
        virtual bool is_public() const
        {
            return m_public;
        }
        virtual bool is_modified() const
        {
            return m_modified;
        }
        virtual void mark_initialized()
        {
            m_modified = false;
        }
        virtual void set_public(bool is_public)
        {
            m_public = is_public;
        }
    protected:
        virtual bool do_parse(const std::string& s) = 0;
        void set_dirty()
        {
            m_modified = true;
        }
        const std::string m_name;
        bool m_public;
        bool m_freezed;
        bool m_modified;
    };
    class string_value_item_parser : public data_item_parser
    {
    public:
        explicit string_value_item_parser(const std::string& name, const std::string& default_val)
            : data_item_parser(name)
            , m_value(default_val)
            , m_default_value(default_val)
        {
        }
        virtual void reset()
        {
            this->m_value = this->m_default_value;
            m_modified = false;
        }
        virtual int get_data_type() const
        {
            return data_type_string;
        }
        virtual void print(output_stream* os) const
        {
            std::ostringstream oss;
            oss << m_value;
            os->write_string(oss.str().c_str());
        }
        virtual const variant* get_value() const
        {
            variant* var = create_variant();
            set_variant_value(var, m_value);
            return var;
        }
        virtual void dump(structured_writer* writer, const char* key) const
        {
            writer->write_string(key, this->m_value);
        }
    protected:
        virtual bool do_parse(const std::string& s)
        {
            this->m_value = xul::strings::trim_copy(s, " \t\r\n");
            return true;
        }
        std::string m_value;
        std::string m_default_value;
    };
    template <typename T, data_type DataItemType>
    class data_item_parser_impl : public data_item_parser
    {
    public:
        explicit data_item_parser_impl(const std::string& name, T& val, T default_val)
            : data_item_parser(name)
            , m_value(val)
            , m_default_value(default_val)
        {
        }
        virtual void reset()
        {
            this->m_value = this->m_default_value;
            m_modified = false;
        }
        virtual int get_data_type() const
        {
            return DataItemType;
        }
        virtual void print(output_stream* os) const
        {
            std::ostringstream oss;
            oss << m_value;
            os->write_string(oss.str().c_str());
        }
        virtual const variant* get_value() const
        {
            variant* var = create_variant();
            set_variant_value(var, m_value);
            return var;
        }
        virtual void dump(structured_writer* writer, const char* key) const
        {
            writer->write(key, this->m_value);
        }
    protected:
        T& m_value;
        T m_default_value;
    };
    class string_item_parser : public data_item_parser_impl<std::string, data_type_string>
    {
    public:
        explicit string_item_parser(const std::string& name, std::string& val, const std::string& default_val)
            : data_item_parser_impl<std::string, data_type_string>(name, val, default_val) { }
        virtual bool do_parse(const std::string& s)
        {
            this->m_value = xul::strings::trim_copy(s, " \t\r\n");
            return true;
        }
        virtual void dump(structured_writer* writer, const char* key) const
        {
            writer->write_string(key, this->m_value);
        }
    };
    template <typename T, data_type DataItemType>
    class number_parser : public data_item_parser_impl<T, DataItemType>
    {
    public:
        explicit number_parser(const std::string& name, T& val, T default_val)
            : data_item_parser_impl<T, DataItemType>(name, val, default_val) { }
        virtual bool do_parse(const std::string& s)
        {
            T val;
            if (!numerics::try_parse<T>(s, val))
                return false;
            this->m_value = val;
            return true;
        }
        virtual void dump(structured_writer* writer, const char* key) const
        {
            writer->write(key, this->m_value);
        }
    };
    template <typename T, data_type DataItemType>
    class general_item_parser : public data_item_parser_impl<T, DataItemType>
    {
    public:
        explicit general_item_parser(const std::string& name, T& val, T default_val)
            : data_item_parser_impl<T, DataItemType>(name, val, default_val) { }
        virtual bool do_parse(const std::string& s)
        {
            if (!this->m_value.try_parse(s))
                return false;
            return true;
        }
    };
    template <typename NumberT>
    class numeric_units
    {
    public:
        typedef NumberT number_type;
        typedef std::map<std::string, NumberT> unit_collection;
        numeric_units()
        {
        }
        void add_unit(const std::string& name, NumberT val)
        {
            this->m_units[name] = val;
        }
        bool try_get_unit(NumberT& val, const std::string& name) const
        {
            return maps::try_get(val, this->m_units, name);
        }
        NumberT get_unit(const std::string& name, NumberT default_unit) const
        {
            return maps::get(this->m_units, name, default_unit);
        }
        NumberT calc_level_unit(NumberT magnitude, int level)
        {
            NumberT val = 1;
            for (int index = 0; index < level; ++index)
            {
                val *= magnitude;
            }
            return val;
        }
        void add_level_unit(const std::string& name, NumberT magnitude, int level)
        {
            this->add_unit(name, calc_level_unit(magnitude, level));
        }
    protected:
        unit_collection m_units;
    };
    class millisecond_time_span_units : public numeric_units<int64_t>, public singleton<const millisecond_time_span_units>
    {
    public:
        millisecond_time_span_units()
        {
            this->add_unit("ms", 1);
            this->add_unit("s", 1000);
            this->add_unit("m", 60 * 1000);
            this->add_unit("h", 60 * 60 * 1000);
        }
    };
    template <typename NumberT>
    class binary_byte_count_units : public numeric_units<NumberT>, public singleton<const binary_byte_count_units<NumberT> >
    {
    public:
        binary_byte_count_units()
        {
            this->do_add_level("K", 1);
            this->do_add_level("M", 2);
            this->do_add_level("G", 3);
            this->do_add_level("T", 4);
            this->do_add_level("P", 5);
        }
    protected:
        void do_add_level(const std::string& name, int level)
        {
            this->add_level_unit(name, 1024, level);
            this->add_level_unit(strings::to_lower_copy(name), 1024, level);
            this->add_level_unit(name + "B", 1024, level);
            this->add_level_unit(strings::to_lower_copy(name) + "B", 1024, level);
        }
    };
    template <typename NumberT>
    class decimal_byte_count_units : public numeric_units<NumberT>, public singleton<const decimal_byte_count_units<NumberT> >
    {
    public:
        decimal_byte_count_units()
        {
            this->do_add_level("K", 1);
            this->do_add_level("M", 2);
            this->do_add_level("G", 3);
            this->do_add_level("T", 4);
            this->do_add_level("P", 5);
        }
    protected:
        void do_add_level(const std::string& name, int level)
        {
            this->add_level_unit(name, 1000, level);
            this->add_level_unit(strings::to_lower_copy(name), 1000, level);
            this->add_level_unit(name + "B", 1000, level);
            this->add_level_unit(strings::to_lower_copy(name) + "B", 1000, level);
            this->add_level_unit(name + "iB", 1024, level);
            this->add_level_unit(strings::to_lower_copy(name) + "iB", 1024, level);
        }
    };
    template <typename T>
    class numeric_units_item_parser : public number_parser<T, data_type_integer>
    {
    public:
        typedef number_parser<T, data_type_integer> base_type;
        explicit numeric_units_item_parser(const std::string& name, T& val, const numeric_units<T>& units, T default_val, const std::string& default_unit_name)
            : number_parser<T, data_type_integer>(name, val, default_val)
            , m_units(units)
            , m_default_unit_name(default_unit_name)
        {
        }
        virtual bool do_parse(const std::string& s)
        {
            if (s.empty())
                return false;
            std::string unit_name;
            size_t pos = s.find_first_not_of("-0123456789");
            if (false == base_type::do_parse(s.substr(0, pos)))
                return false;
            if (std::string::npos != pos)
                unit_name = s.substr(pos);
            T unit_value = 1;
            if (unit_name.size() > 0 && false == this->m_units.try_get_unit(unit_value, unit_name))
                return false;
            this->m_value *= unit_value;
            return true;
        }
    protected:
        const numeric_units<T>& m_units;
        const std::string m_default_unit_name;
    };
    class switch_parser : public data_item_parser_impl<bool, data_type_bool>
    {
    public:
        explicit switch_parser(const std::string& name, bool& val, bool default_val)
            : data_item_parser_impl<bool, data_type_bool>(name, val, default_val) { }
        virtual bool do_parse(const std::string& s)
        {
            std::string val = xul::strings::trim_copy(s, " \t\r\n");
            xul::strings::make_lower(val);
            if ("on" == val || "true" == val || "yes" == val || "1" == val)
                this->m_value = true;
            else if ("off" == val || "false" == val || "no" == val || "0" == val)
                this->m_value = false;
            else
                return false;
            return true;
        }
        virtual void print(output_stream* os) const
        {
            os->write_string(m_value ? "true" : "false");
        }
        virtual void dump(structured_writer* writer, const char* key) const
        {
            writer->write_bool(key, this->m_value);
        }
    };
    class bool_parser : public data_item_parser_impl<bool, data_type_bool>
    {
    public:
        explicit bool_parser(const std::string& name, bool& val, bool default_val)
            : data_item_parser_impl<bool, data_type_bool>(name, val, default_val) { }
        virtual bool do_parse(const std::string& s)
        {
            std::string val = xul::strings::trim_copy(s, " \t\r\n");
            xul::strings::make_lower(val);
            if (val == "true" || "yes" == val)
                this->m_value = true;
            else if (val == "false" || "no" == val)
                this->m_value = false;
            else
                return false;
            return true;
        }
        virtual void print(output_stream* os) const
        {
            os->write_string(m_value ? "true" : "false");
        }
        virtual void dump(structured_writer* writer, const char* key) const
        {
            writer->write_bool(key, this->m_value);
        }
    };
    template <typename EnumT>
    class enum_parser : public data_item_parser_impl<typename EnumT::enum_type, data_type_integer>
    {
    public:
        explicit enum_parser(const std::string& name, typename EnumT::enum_type& val, typename EnumT::enum_type default_val)
            : data_item_parser_impl<typename EnumT::enum_type, data_type_integer>(name, val, default_val) { }
        virtual bool do_parse(const std::string& s)
        {
            if (!EnumT::try_parse(s, this->m_value))
                return false;
            return true;
        }
    };

    void add_string(const std::string& name, std::string* item, const std::string& default_val)
    {
        do_add(name, new string_item_parser(name, *item, default_val));
    }
    void add(const std::string& name, std::string* item, const std::string& default_val)
    {
        do_add(name, new string_item_parser(name, *item, default_val));
    }
    void add(const std::string& name, std::string* item, const char* default_val = "")
    {
        do_add(name, new string_item_parser(name, *item, default_val));
    }
    void add(const std::string& name, bool* item, bool default_val)
    {
        do_add(name, new bool_parser(name, *item, default_val));
    }
    void add_switch(const std::string& name, bool* item, bool default_val)
    {
        do_add(name, new switch_parser(name, *item, default_val));
    }
    template <typename EnumT>
    void add_enum(const std::string& name, typename EnumT::enum_type& item, typename EnumT::enum_type default_val)
    {
        do_add(name, new enum_parser<EnumT>(name, item, default_val));
    }
    template <typename NumberT>
    void add_unit(const std::string& name, NumberT* item, const numeric_units<NumberT>& units, NumberT default_val, const std::string& default_unit_name = std::string())
    {
        do_add(name, new numeric_units_item_parser<NumberT>(name, *item, units, default_val, default_unit_name));
    }
    template <typename NumberT>
    void add_decimal_byte_count(const std::string& name, NumberT* item, NumberT default_val, const std::string& default_unit_name = std::string())
    {
        add_unit<NumberT>(name, item, decimal_byte_count_units<NumberT>::instance(), default_val, default_unit_name);
    }
    template <typename NumberT>
    void add_binary_byte_count(const std::string& name, NumberT* item, NumberT default_val, const std::string& default_unit_name = std::string())
    {
        add_unit<NumberT>(name, item, binary_byte_count_units<NumberT>::instance(), default_val, default_unit_name);
    }
    template <typename T>
    void add(const std::string& name, T* item, const T& default_val)
    {
        do_add(name, new number_parser<T, data_type_integer>(name, *item, default_val));
    }
#if 0
    template <typename T>
    void add(const std::string& name, T* item, const T& default_val)
    {
        do_add(name, new lexical_item_parser<T, data_type_integer>(name, *item, default_val));
    }
    template <typename T>
    void add_object(const std::string& name, T* item, const T& default_val)
    {
        do_add(name, new general_item_parser<T, data_type_string>(name, *item, default_val));
    }
#endif

    bool parse(const std::string& name, const std::string& val)
    {
        std::string realname = xul::strings::trim_copy(name, " \t\r\n");
        std::string realval = xul::strings::trim_copy(val, " \t\r\n");
        data_item_parser_ptr parser = maps::get(m_item_parsers, realname);
        if (!parser)
            return false;
        return parser->parse(realval.c_str());
    }

    bool parse(const std::string& expr, char sep = '=')
    {
        std::pair<std::string, std::string> keyval = strings::split_pair(expr, sep);;
        return parse(keyval.first, keyval.second);
    }
    void reset()
    {
        XUL_FOREACH_CONST(item_parser_collection, m_item_parsers, iter)
        {
            data_item_parser_ptr parser = iter->second;
            parser->reset();
        }
    }

protected:
    void do_add(const std::string& name, data_item_parser* parser)
    {
        m_item_parsers[name] = data_item_parser_ptr(parser);
        parser->reset();
    }

protected:
    item_parser_collection m_item_parsers;
};


}
