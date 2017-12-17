#pragma once

#include <xul/data/pool_buffer.hpp>
#include <xul/data/pool.hpp>


namespace xul {


class block_pool : public pool
{
public:
    virtual int get_block_size() const = 0;
    virtual int get_used_count() const = 0;
    virtual int get_max_count() const = 0;
    virtual char* allocate() = 0;
    virtual void deallocate(char* buf) = 0;

    pool_buffer_ptr create()
    {
        return pool_buffer_ptr(new pool_buffer(*this, this->allocate(), get_block_size()));
    }
};


}
