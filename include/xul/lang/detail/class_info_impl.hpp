#pragma once

#include <xul/lang/class_info.hpp>
#include <xul/lang/object_impl.hpp>
#include <string>
#include <assert.h>


namespace xul {


class class_info_impl : public object_impl<class_info>
{
public:
    explicit class_info_impl(const std::string& name) : m_name(name) { }

    virtual const char* get_name() const { return m_name.c_str(); }
    virtual object* create_object()
    {
        assert(false);
        return NULL;
    }

private:
    std::string m_name;
};


}
