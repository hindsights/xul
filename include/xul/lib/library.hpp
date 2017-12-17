#pragma once

#include <xul/lib/global_service_manager.hpp>


namespace xul {


class library : public object
{
public:
    virtual global_service_manager* get_service_manager() = 0;
};


}
