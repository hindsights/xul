#pragma once

#include <vector>
#include <string>
#include <stddef.h>


namespace xul {


class inet4_address;
class name_resolver_listener;
class name_resolver;


class name_resolver_listener
{
public:
    virtual ~name_resolver_listener() { }

    virtual void on_resolver_address(name_resolver* sender, const std::string& name, int errcode, const std::vector<inet4_address>& addrs) = 0;
};


}
