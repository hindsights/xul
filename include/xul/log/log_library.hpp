#pragma once

#include <xul/lib/library.hpp>


namespace xul {


class log_service;
class log_configurator;


class log_library : public library
{
public:
    virtual log_service* create_service() = 0;
    virtual log_configurator* create_properties_configurator() = 0;
};


}
