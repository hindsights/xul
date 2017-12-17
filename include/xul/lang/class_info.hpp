#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class class_info : public object
{
public:
    virtual const char* get_name() const = 0;
    virtual object* create_object() = 0;
};


class_info* create_class_info(const char* name);


}
