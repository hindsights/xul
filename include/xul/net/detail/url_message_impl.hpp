#pragma once

#include <xul/net/http/http_message.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul { namespace detail {


template <typename BaseT>
class url_message_impl : public object_impl<BaseT>
{
public:
    virtual const char* get_protocol_name() const
    {
        return this->m_message.get_protocol_name();
    }
    virtual void set_protocol_name(const char* name)
    {
        this->m_message.set_protocol_name(name);
    }

    virtual const char* get_protocol_version() const
    {
        return this->m_message.get_protocol_version();
    }
    virtual void set_protocol_version(const char* val)
    {
        this->m_message.set_protocol_version(val);
    }

    virtual const char* get_header(const char* name, const char* default_val) const
    {
        return this->m_message.get_header(name, default_val);
    }
    virtual void set_header(const char* name, const char* val)
    {
        this->m_message.set_header(name, val);
    }

protected:
    http_message m_message;
};


} }
