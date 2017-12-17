#pragma once

#include <xul/lang/object.hpp>
#include <xul/util/listenable.hpp>
#include <xul/net/name_resolver_listener.hpp>
#include <string>
#include <vector>

namespace xul {


class inet4_address;
class name_resolver_listener;
class name_resolver;

class name_resolver : public object, public listenable<name_resolver_listener>
{
public:
    virtual ~name_resolver() { }

    virtual void async_resolve(const std::string& name) = 0;
    virtual bool resolve(const std::string& name, std::vector<inet4_address>& addresses) = 0;
    virtual void close() = 0;
    virtual void destroy() = 0;
};

}
