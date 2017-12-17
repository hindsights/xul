#pragma once

#include <xul/lang/object_impl.hpp>
#include <xul/lib/global_service_manager.hpp>
#include <xul/log/logging.hpp>


namespace xul {


class log_service;

class global_service_manager_impl : public object_impl<global_service_manager>
{
public:
    virtual void set_default_log_service(log_service* svc)
    {
        logging::set_service(svc);
    }
};


}
