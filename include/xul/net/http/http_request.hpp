#pragma once

#include <xul/net/url_request.hpp>
#include <xul/net/detail/url_message_impl.hpp>
#include <xul/net/uri.hpp>
#include <xul/io/output_streams.hpp>
#include <string>


namespace xul {


/// A request received from a client.
class http_request : public detail::url_message_impl<url_request>
{
public:
    explicit http_request(uri* u = create_uri(), uri* encoded_u = create_uri()) : m_uri(u), m_encoded_uri(encoded_u)
    {
        clear();
    }

    virtual url_request* clone() const
    {
        http_request* new_copy = new http_request(this->m_uri->clone(), this->m_encoded_uri->clone());
        new_copy->m_method = this->m_method;
        new_copy->m_message.set_protocol_name(this->m_message.get_protocol_name());
        new_copy->m_message.set_protocol_version(this->m_message.get_protocol_version());
        new_copy->m_message.set_headers(this->m_message.get_headers()->clone());
        return new_copy;
    }

    virtual void clear()
    {
        this->m_method.clear();
        this->m_uri->clear();
        this->m_encoded_uri->clear();
        this->m_message.clear();
    }

    virtual const char* get_method() const
    {
        return m_method.c_str();
    }
    virtual void set_method(const char* m)
    {
        m_method = m;
    }

    virtual const uri* get_uri() const
    {
        return m_uri.get();
    }
    virtual const char* get_url() const
    {
        return m_uri->get_original_string();
    }
    virtual void set_url(const char* s)
    {
        if (s)
        {
            m_uri->parse(s);
            m_encoded_uri->parse(s);
            m_encoded_uri->encode();
        }
        else
        {
            m_uri->clear();
            m_encoded_uri->clear();
        }
    }

    virtual void print(output_stream* os) const
    {
        *os << m_method << " " << m_encoded_uri->get_original_string() << " " << m_message.get_protocol_name() << "/" << m_message.get_protocol_version() << "\r\n";
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
    std::string m_method;
    boost::intrusive_ptr<uri> m_uri;
    boost::intrusive_ptr<uri> m_encoded_uri;
};


}
