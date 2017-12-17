#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/list.hpp>


namespace xul {


class dns_backup : public object
{
public:
    static dns_backup* get_global_backup();

    virtual void clear() = 0;
    virtual bool remove(const char* hostname) = 0;
    virtual bool add(const char* hostname, const char* ip) = 0;
    virtual bool add_ipv4(const char* hostname, uint32_t ip) = 0;
    virtual list<uint32_t>* lookup(const char* hostname) const = 0;
};


}
