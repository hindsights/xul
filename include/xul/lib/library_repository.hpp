#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class class_info;
class library_info;
class library;


class library_repository : public object
{
public:
    virtual bool register_class(class_info* info) = 0;
    virtual bool register_library(library_info* info) = 0;
    virtual class_info* find_class(const char* name) = 0;
    virtual object* create_object(const char* name) = 0;
    virtual library_info* find_library(const char* name) = 0;
    virtual library* load_library(const char* name) = 0;
};


}
