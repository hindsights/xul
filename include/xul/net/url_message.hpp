#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/printable.hpp>
#include <xul/data/string_table.hpp>


namespace xul {


/// base class of url_request/url_response
class url_message : public object, public printable
{
public:
    virtual void clear() = 0;

    virtual const char* get_protocol_name() const = 0;
    virtual void set_protocol_name(const char* name) = 0;

    virtual const char* get_protocol_version() const = 0;
    virtual void set_protocol_version(const char* val) = 0;

    virtual const string_table* get_headers() const = 0;
    virtual string_table* ref_headers() = 0;
    virtual void set_headers(const string_table* headers) = 0;

    virtual const char* get_header(const char* key, const char* default_val) const = 0;
    virtual void set_header(const char* key, const char* val) = 0;
};


}
