#pragma once

#include <xul/text/structured_text_writer.hpp>
#include <xul/io/output_streams.hpp>


namespace xul {


class json_writer : public structured_text_writer
{
public:
    explicit json_writer(output_stream* out, bool compressed) : structured_text_writer(out, compressed)
    {
        set_linefeed("\n");
        set_indentation(2);
    }
    virtual void start_scalar(const char* name)
    {
        writer_context* upper_context = get_context();
        if (upper_context)
        {
            if (false == upper_context->is_first_item)
                *m_out << ",";
            upper_context->is_first_item = false;
        }
        write_linefeed();
        write_indentation();
        if (NULL == upper_context || upper_context->element_type != array_element)
        {
            *m_out << "\"" << name << "\" : ";
        }
        else
        {
        }
    }
    virtual void end_scalar(const char* name)
    {
    }

    virtual void write_element_start(const char* name, element_type_enum elemType, writer_context& context)
    {
        writer_context* upper_context = get_context();
        if (m_contexts.empty())
        {
            write_object_start_mark();
            increase_indentation();
        }
        else
        {
            if (false == upper_context->is_first_item)
            {
                *m_out << ",";
            }
            upper_context->is_first_item = false;
        }

        write_linefeed();
        write_indentation();
        // for array item, 'name' tag isnot needed
        if (NULL == upper_context || upper_context->element_type != array_element)
        {
            *m_out << "\"" << name << "\" : ";
        }

        if (object_element == elemType)
        {
            write_object_start_mark();
            increase_indentation();
        }
        else if (array_element == elemType)
        {
            write_array_start_mark();
            increase_indentation();
        }
        //m_current_context = &context;
        m_contexts.push(context);
    }
    virtual void write_element_end(const char* name, element_type_enum elemType, writer_context& context)
    {
        //m_current_context = upperContext;
        m_contexts.pop();
        if (elemType != simple_element)
        {
            decrease_indentation();
            write_linefeed();
            write_indentation();
            if (object_element == elemType)
            {
                write_object_end_mark();
            }
            else if (array_element == elemType)
            {
                write_array_end_mark();
            }
        }
        if (m_contexts.empty())
        {
            decrease_indentation();
            write_linefeed();
            write_object_end_mark();
        }
    }

    virtual void write_string_value(const char* val)
    {
        *m_out << '"' << (val ? val : "") << '"';
    }

protected:
    void write_object_start_mark()
    {
        *m_out << "{";
        //increase_indentation();
    }
    void write_object_end_mark()
    {
        //decrease_indentation();
        *m_out << "}";
    }

    void write_array_start_mark()
    {
        *m_out << "[ ";
    }
    void write_array_end_mark()
    {
        *m_out << "]";
    }
};


}
