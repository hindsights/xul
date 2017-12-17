#pragma once

#include <xul/net/name_resolver_listener.hpp>
#include <assert.h>

namespace xul {
namespace detail {


class dummy_name_resolver_listener : public name_resolver_listener
{
public:
    virtual void on_resolver_address(name_resolver* sender, const std::string& name, int errcode, const std::vector<inet4_address>& addrs) { }
};

class checked_name_resolver_listener : public name_resolver_listener
{
public:
    virtual void on_resolver_address(name_resolver* sender, const std::string& name, int errcode, const std::vector<inet4_address>& addrs) { assert(false); }
};


}
}
