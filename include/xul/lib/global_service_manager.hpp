#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class log_service;

class global_service_manager : public object
{
public:
    virtual void set_default_log_service(log_service*) = 0;
};


}
