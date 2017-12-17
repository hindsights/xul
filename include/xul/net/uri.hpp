#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/printable.hpp>
#include <xul/data/string_table.hpp>
#include <iosfwd>
#include <string>


namespace xul {


/// interface for uri
class uri : public object, public printable
{
public:
    virtual const char* get_original_string() const = 0;
    virtual const char* get_scheme() const = 0;
    virtual const char* get_host() const = 0;
    virtual int get_port() const = 0;
    virtual const char* get_username() const = 0;
    virtual const char* get_password() const = 0;

    /// path and query part
    virtual const char* get_full_path() const = 0;
    virtual const char* get_path() const = 0;
    virtual const char* get_filename() const = 0;
    virtual const char* get_query() const = 0;
    virtual const char* get_fragment() const = 0;
    virtual const string_table* get_params() const = 0;

    virtual const char* get_param(const char* name) const = 0;
    virtual bool has_param(const char* name) const = 0;
    virtual void set_param(const char* name, const char* val) = 0;
    virtual void remove_param(const char* name) = 0;

    virtual void clear() = 0;
    virtual bool parse(const char* s) = 0;

    virtual uri* clone() const = 0;
    virtual void assemble(output_stream* os) const = 0;
    virtual void encode() = 0;
};


uri* create_uri();
std::ostream& operator<<(std::ostream& os, const uri& url);


}
