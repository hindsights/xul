#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/list.hpp>
#include <xul/data/map.hpp>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include <iosfwd>


namespace xul {


template <typename T>
class list;

template <typename KeyT, typename ValueT>
class map;

enum
{
    variant_type_null = 0,
    variant_type_bool = 1,
    variant_type_integer = 2,
    variant_type_float = 3,
    variant_type_string = 4,
    variant_type_pointer = 5,
    variant_type_object = 6,
    variant_type_list = 7,
    variant_type_dict = 8,
    variant_type_taged_buffer = 9,
};


class variant;
typedef list<variant*> variant_list;
typedef map<const char*, variant*> variant_dict;


class variant_tagged_buffer : public object
{
public:
    virtual const char* get_tag() const = 0;
    virtual const uint8_t* get_data() const = 0;
    virtual int get_size() const = 0;
    virtual void assign(const uint8_t* buf, int bufsize) = 0;
};


class variant : public object
{
public:
    typedef variant_list list_type;
    typedef variant_dict dict_type;

    virtual int get_type() const = 0;

    virtual bool is_null() const = 0;
    virtual void set_null() = 0;

    virtual bool is_bool() const = 0;
    virtual bool get_bool(bool default_val) const = 0;
    virtual bool try_get_bool(bool* outval) const = 0;
    virtual void set_bool(bool val) = 0;

    virtual bool is_integer() const = 0;
    virtual int64_t get_integer(int64_t default_val) const = 0;
    virtual bool try_get_integer(int64_t* outval) const = 0;
    virtual void set_integer(int64_t val) = 0;

    virtual bool is_float() const = 0;
    virtual double get_float(double default_val) const = 0;
    virtual bool try_get_float(double* outval) const = 0;
    virtual void set_float(double val) = 0;

    virtual bool is_string() const = 0;
    virtual const char* get_string(const char* default_val) const = 0;
    virtual const char* get_bytes(int* len) const = 0;
    virtual void set_string(const char* val) = 0;
    virtual void set_bytes(const char* val, int len) = 0;

    virtual bool is_pointer() const = 0;
    virtual void* get_pointer() const = 0;
    virtual void set_pointer(void* val) = 0;

    virtual bool is_object() const = 0;
    virtual object* get_object() const = 0;
    virtual void set_object(object* val) = 0;

    virtual bool is_list() const = 0;
    virtual const variant_list* get_list() const = 0;
    virtual variant_list* ref_list() = 0;
    virtual variant_list* set_list() = 0;

    virtual bool is_dict() const = 0;
    virtual const variant_dict* get_dict() const = 0;
    virtual variant_dict* ref_dict() = 0;
    virtual variant_dict* set_dict() = 0;

    virtual bool is_tagged_buffer() const = 0;
    virtual void set_tagged_buffer(variant_tagged_buffer* val) = 0;
    virtual const variant_tagged_buffer* get_tagged_buffer() const = 0;
};


variant* create_variant();
variant_list* create_variant_list();
variant_dict* create_variant_dict();
std::ostream& operator<<(std::ostream& os, const variant& var);
std::ostream& operator<<(std::ostream& os, const variant_list& vars);


inline variant* create_null_variant()
{
    variant* var = create_variant();
    var->set_null();
    return var;
}

inline variant* create_bool_variant(bool val)
{
    variant* var = create_variant();
    var->set_bool(val);
    return var;
}

inline variant* create_string_variant(const char* str)
{
    variant* var = create_variant();
    var->set_string(str);
    return var;
}

inline variant* create_integer_variant(int64_t val)
{
    variant* var = create_variant();
    var->set_integer(val);
    return var;
}

inline variant* create_float_variant(double val)
{
    variant* var = create_variant();
    var->set_float(val);
    return var;
}


inline void set_variant_value(variant* var, bool val)
{
    var->set_bool(val);
}
inline void set_variant_value(variant* var, const char* val)
{
    var->set_string(val);
}
inline void set_variant_value(variant* var, const std::string& val)
{
    var->set_bytes(val.c_str(), val.size());
}
inline void set_variant_value(variant* var, int val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, unsigned int val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, short val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, unsigned short val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, long val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, unsigned long val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, long long val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, unsigned long long val)
{
    var->set_integer(val);
}
inline void set_variant_value(variant* var, float val)
{
    var->set_float(val);
}
inline void set_variant_value(variant* var, double val)
{
    var->set_float(val);
}


}


