#pragma once

#include <xul/lib/library.hpp>


namespace xul {


class io_service;


class io_library : public library
{
public:
    virtual io_service* create_service() = 0;
};


}
