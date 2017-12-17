#pragma once

#include <xul/text/structured_text_writer.hpp>
#include <xul/text/xml_escape_sequence.hpp>
#include <xul/io/output_streams.hpp>
#include <xul/std/maps.hpp>
#include <xul/macro/foreach.hpp>


namespace xul {


class xml_writer : public structured_text_writer
{
public:
    explicit xml_writer(output_stream* out, bool compressed, const std::string& encoding = "utf-8")
        : structured_text_writer(out, compressed)
        , m_encoding(encoding)
    {
    }

    virtual void start_scalar(const char* name)
    {
        write_linefeed();
        write_indentation();
        *m_out << "<" << name << ">";
    }
    virtual void end_scalar(const char* name)
    {
        *m_out << "</" << name << ">";
    }
    virtual void write_element_start(const char* name, element_type_enum elemType, writer_context& context)
    {
        // xml format does not care about isArrayItem and isFirstItem
        if (m_contexts.empty())
        {
            *m_out << "<?xml version=\"1.0\" encoding=\"" << m_encoding << "\" standalone=\"yes\" ?>" << "\n";
        }
        else
        {
            write_linefeed();
        }
        write_indentation();
        if (array_element == elemType)
        {
            *m_out << "<" << name << " type=\"array\" count=\"" << context.array_size << "\">";
        }
        else
        {
            *m_out << "<" << name << ">";
        }
        if (elemType != simple_element)
        {
            increase_indentation();
        }
        //m_current_context = &context;
        m_contexts.push(context);
    }
    virtual void write_element_end(const char* name, element_type_enum elemType, writer_context& context)
    {
        m_contexts.pop();
        //m_current_context = upperContext;
        // xml format does not care about isArrayItem, isFirstItem and isTopLevel
        if (elemType != simple_element)
        {
            decrease_indentation();
            write_linefeed();
            write_indentation();
        }
        *m_out << "</" << name << ">";
    }

    virtual void write_string_value(const char* val)
    {
        xml_escape_sequence::write(m_out, val ? val : "");
    }

private:
    std::string m_encoding;
};
    


}
