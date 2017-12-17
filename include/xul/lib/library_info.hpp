#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class library_repository;
class library;

class library_info : public object
{
public:
    virtual const char* get_name() const = 0;
    virtual unsigned int get_version() const = 0;
    virtual library* load_library() = 0;
    virtual int register_classes(library_repository* repo) = 0;
};


}
