#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/variant.hpp>
#include <xul/data/data_type.hpp>
#include <xul/data/printable.hpp>

/**
 * @file
 * @brief option item interface
 */


namespace xul {


class structured_writer;


class option : public object, public printable
{
public:
    virtual const char* get_name() const = 0;
    virtual const variant* get_value() const = 0;
    virtual bool parse(const char* s) = 0;
    virtual void reset() = 0;
    virtual int get_data_type() const = 0;
    virtual void freeze() = 0;
    virtual bool is_freezed() const = 0;
    virtual bool is_public() const = 0;
    virtual void set_public(bool is_public) = 0;
    virtual bool is_modified() const = 0;
    virtual void mark_initialized() = 0;
    virtual void dump(structured_writer* writer, const char* key) const = 0;
};


}
