#pragma once

#include <xul/net/url_response.hpp>
#include <xul/net/detail/url_message_impl.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/net/http/http_message.hpp>
#include <xul/net/http/http_status_code.hpp>
#include <xul/data/numerics.hpp>


namespace xul {


/// A response received from a client.
class http_response : public detail::url_message_impl<url_response>
{
public:
    http_response()
    {
        clear();
    }

    virtual int get_status_code() const
    {
        return m_status_code;
    }

    virtual void set_status_code(int status_code)
    {
        m_status_code = status_code;
        std::string desc = http_status_code::get_description(m_status_code);
        if (false == desc.empty())
            m_status_description = desc;
    }

    virtual const char* get_status_description() const
    {
        return m_status_description.c_str();
    }

    virtual void set_status_description(const char* val)
    {
        m_status_description = val ? val : "";
    }

    virtual void clear()
    {
        m_message.clear();
        m_status_code = http_status_code::none;
    }

    virtual void print(output_stream* os) const
    {
        *os << m_message.get_protocol_name() << '/'
            << m_message.get_protocol_version() << ' '
            << numerics::format<int>(m_status_code) << ' '
            << m_status_description << "\r\n";
        m_message.write_headers(*os);
    }

    virtual const string_table* get_headers() const
    {
        return m_message.get_headers();
    }
    virtual string_table* ref_headers()
    {
        return m_message.ref_headers();
    }
    virtual void set_headers(const string_table* header)
    {
        m_message.set_headers(header);
    }
    virtual const char* get_header(const char* key, const char* default_val) const
    {
        return m_message.get_headers()->get(key, default_val);
    }
    virtual void add_header(const char* key, const char* val)
    {
        return m_message.ref_headers()->set(key, val);
    }

private:
    std::string m_status_description;
    int m_status_code;
};


}
