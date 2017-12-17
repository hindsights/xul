#pragma once

#include <xul/data/variant.hpp>
#include <xul/lang/object_impl.hpp>
#include <string>


namespace xul { namespace detail {


class variant_impl : public object_impl<variant>
{
public:
    typedef list<variant*> variant_list_type;
    typedef map<const char*, variant*> variant_dict_type;

    variant_impl()
    {
        m_type = variant_type_null;
    }

    virtual int get_type() const
    {
        return m_type;
    }

    virtual bool is_null() const
    {
        return variant_type_null == m_type;
    }
    virtual void set_null()
    {
        m_type = variant_type_null;
    }

    virtual bool is_bool() const
    {
        return variant_type_bool == m_type;
    }
    virtual bool get_bool(bool default_val) const
    {
        assert(is_bool());
        return is_bool() ? m_bool : default_val;
    }
    virtual bool try_get_bool(bool* outval) const
    {
        assert(is_bool());
        if (is_bool() && outval)
        {
            *outval = m_bool;
            return true;
        }
        return false;
    }
    virtual void set_bool(bool val)
    {
        m_bool = val;
        m_type = variant_type_bool;
    }

    virtual bool is_integer() const
    {
        return variant_type_integer == m_type;
    }
    virtual int64_t get_integer(int64_t default_val) const
    {
        return is_integer() ? m_integer : default_val;
    }
    virtual bool try_get_integer(int64_t* outval) const
    {
        assert(is_integer());
        if (is_integer() && outval)
        {
            *outval = m_integer;
            return true;
        }
        return false;
    }
    virtual void set_integer(int64_t val)
    {
        m_integer = val;
        m_type = variant_type_integer;
    }

    virtual bool is_float() const
    {
        return variant_type_float == m_type;
    }
    virtual double get_float(double default_val) const
    {
        return is_float() ? m_float : default_val;
    }
    virtual bool try_get_float(double* outval) const
    {
        assert(is_float());
        if (is_float() && outval)
        {
            *outval = m_float;
            return true;
        }
        return false;
    }
    virtual void set_float(double val)
    {
        m_float = val;
        m_type = variant_type_float;
    }

    virtual bool is_string() const
    {
        return variant_type_string == m_type;
    }
    virtual const char* get_string(const char* default_val) const
    {
        return is_string() ? m_string.c_str() : default_val;
    }
    virtual const char* get_bytes(int* len) const
    {
        if (!is_string())
            return NULL;
        if (len)
        {
            *len = m_string.size();
        }
        return m_string.c_str();
    }
    virtual void set_string(const char* val)
    {
        m_string = val;
        m_type = variant_type_string;
    }
    virtual void set_bytes(const char* val, int len)
    {
        m_string.assign(val, len);
        m_type = variant_type_string;
    }

    virtual bool is_pointer() const
    {
        return variant_type_pointer == m_type;
    }
    virtual void* get_pointer() const
    {
        return is_pointer() ? m_pointer : NULL;
    }
    virtual void set_pointer(void* val)
    {
        m_pointer = val;
        m_type = variant_type_pointer;
    }

    virtual bool is_object() const
    {
        return variant_type_object == m_type;
    }
    virtual object* get_object() const
    {
        return is_object() ? m_object.get() : NULL;
    }
    virtual void set_object(object* val)
    {
        m_object = val;
        m_type = variant_type_object;
    }

    virtual bool is_list() const
    {
        return variant_type_list == m_type;
    }
    virtual const variant_list_type* get_list() const
    {
        return is_list() ? m_list.get() : NULL;
    }
    virtual variant_list_type* ref_list()
    {
        return is_list() ? m_list.get() : NULL;
    }
    virtual list_type* set_list()
    {
        m_list = create_variant_list();
        m_type = variant_type_list;
        return m_list.get();
    }

    virtual bool is_dict() const
    {
        return variant_type_dict == m_type;
    }
    virtual const variant_dict_type* get_dict() const
    {
        return is_dict() ? m_dict.get() : NULL;
    }
    virtual variant_dict_type* ref_dict()
    {
        return is_dict() ? m_dict.get() : NULL;
    }
    virtual dict_type* set_dict()
    {
        m_dict = create_variant_dict();
        m_type = variant_type_dict;
        return m_dict.get();
    }

    virtual bool is_tagged_buffer() const
    {
        return m_type == variant_type_taged_buffer;
    }
    virtual void set_tagged_buffer(variant_tagged_buffer* val)
    {
        assert(val);
        m_tagged_buffer = val;
        m_type = variant_type_taged_buffer;
    }
    virtual const variant_tagged_buffer* get_tagged_buffer() const
    {
        return is_tagged_buffer() ? m_tagged_buffer.get() : NULL;
    }

private:
    int m_type;
    bool m_bool;
    int64_t m_integer;
    double m_float;
    std::string m_string;
    void* m_pointer;
    boost::intrusive_ptr<variant_tagged_buffer> m_tagged_buffer;
    boost::intrusive_ptr<object> m_object;
    boost::intrusive_ptr<variant_list_type> m_list;
    boost::intrusive_ptr<variant_dict_type> m_dict;
};


} }
