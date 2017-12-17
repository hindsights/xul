#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class string_tokenizer : public object
{
public:
    virtual void reset() = 0;
    virtual void set_max_buffer_size(size_t size) = 0;
    virtual void set_delimiter(const char* data, size_t size) = 0;
    virtual void set_include_delimiter(bool includes) = 0;
    virtual bool is_delimiter_included() const = 0;
    virtual void feed(const char* data, size_t size) = 0;
    virtual bool next_token(const char** pos, size_t* len) = 0;
    virtual bool get_remains(const char** pos, size_t* len) = 0;
};


string_tokenizer* create_string_tokenizer();


}
