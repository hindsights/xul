#pragma once

#include <xul/io/structured_writer.hpp>
#include <xul/std/strings.hpp>
#include <xul/macro/minmax.hpp>
#include <xul/io/output_stream.hpp>
#include <xul/lang/object_impl.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <stack>
#include <stdint.h>

namespace xul {


class structured_text_writer : public object_impl<structured_writer>
{
public:
    enum element_type_enum
    {
        simple_element = 1,
        object_element = 2,
        array_element = 3,
    };

    virtual void write(const variant* var)
    {
    }
    virtual void write_list(const variant::list_type* vars)
    {
    }
    virtual void write_dict(const variant::dict_type* vars)
    {
    }

    class writer_context
    {
    public:
        std::string name;
        bool is_first_item;
        int array_size;
        element_type_enum element_type;

        writer_context()
        {
            is_first_item = false;
            array_size = -1;
            element_type = simple_element;
        }
    };

    explicit structured_text_writer(output_stream* out, bool compressed)
        : m_out(out)
        , m_compressed(compressed)
        , m_indentation_unit(0)
        , m_current_indentation(0)
        //, m_current_context(NULL)
        //, m_need_indentation(false)
        //, m_need_linefeed(false)
    {
        assert(m_out);
        set_linefeed("\n");
        set_indentation(2);
    }
    virtual ~structured_text_writer() { }


    output_stream* get_stream()
    {
        return m_out;
    }

    virtual void write_bool_value(bool val)
    {
        write_simple_value(val ? "true" : "false");
    }
    virtual void write_int_value(int val)
    {
        write_simple_value(val);
    }
    virtual void write_uint_value(unsigned int val)
    {
        write_simple_value(val);
    }
    virtual void write_int64_value(int64_t val)
    {
        write_simple_value(val);
    }
    virtual void write_uint64_value(uint64_t val)
    {
        write_simple_value(val);
    }
    virtual void write_float_value(float val)
    {
        write_simple_value(val);
    }
    virtual void write_double_value(double val)
    {
        write_simple_value(val);
    }
    virtual void write_long_double_value(long double val)
    {
        write_simple_value(val);
    }
    virtual void write_simple_value(const char* val)
    {
        m_out->write_string(val);
    }

    void set_linefeed(const char* linefeed)
    {
        m_linefeed = linefeed;
    }
    void set_indentation(int indentation)
    {
        m_indentation_unit = indentation;
        XUL_LIMIT_MIN_MAX(m_indentation_unit, 0, 256);
    }

    virtual void write_string_value(const char* val) = 0;

    void write_simple_value(const std::string& val)
    {
        write_simple_value(val.c_str());
    }
    template <typename T>
    void write_simple_value(const T& val)
    {
        std::ostringstream os;
        os << val;
        write_simple_value(os.str());
    }

    writer_context* get_context()
    {
        return m_contexts.empty() ? NULL : &m_contexts.top();
    }

    virtual void start_dict(const char* name)
    {
        writer_context context;
        context.is_first_item = true;
        context.element_type = object_element;
        context.array_size = -1;
        context.name = name;
        write_element_start(name, object_element, context);
    }
    virtual void end_dict()
    {
        if (m_contexts.empty())
        {
            assert(false);
            return;
        }
        writer_context context = m_contexts.top();
        assert(object_element == context.element_type);
        write_element_end(context.name.c_str(), context.element_type, context);
    }
    virtual void start_list(const char* name, int count)
    {
        writer_context context;
        context.is_first_item = true;
        context.element_type = array_element;
        context.array_size = count;
        context.name = name;
        write_element_start(name, array_element, context);
    }
    virtual void end_list()
    {
        if (m_contexts.empty())
        {
            assert(false);
            return;
        }
        writer_context context = m_contexts.top();
        assert(array_element == context.element_type);
        write_element_end(context.name.c_str(), context.element_type, context);
    }

protected:
    virtual void write_element_start(const char* name, element_type_enum elemType, writer_context& context) = 0;
    virtual void write_element_end(const char* name, element_type_enum elemType, writer_context& context) = 0;

    void write_indentation()
    {
        if (m_compressed)
            return;
        if (m_current_indentation > 0)
        {
            std::string indentation(m_current_indentation, ' ');
            m_out->write_chars(indentation.c_str(), indentation.size());
        }
    }
    void increase_indentation()
    {
        m_current_indentation += m_indentation_unit;
    }
    void decrease_indentation()
    {
        m_current_indentation -= m_indentation_unit;
    }
    void write_linefeed()
    {
        if (m_compressed)
            return;
        m_out->write_chars(m_linefeed.c_str(), m_linefeed.size());
    }

protected:
    output_stream* m_out;
    bool m_compressed;
    std::string m_linefeed;
    int m_indentation_unit;
    int m_current_indentation;
    /// stack of history contexts
    std::stack<writer_context> m_contexts;
};


}
