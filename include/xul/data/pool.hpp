#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class pool : public object
{
public:
    virtual ~pool() { }

    virtual void deallocate(char* buf) = 0;
    virtual bool is_from(const char* buf, size_t size) const = 0;
    virtual void release_memory() = 0;
};


}
