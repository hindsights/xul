#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class id_generator : public xul::object
{
public:
    virtual uint32_t generate() = 0;
    virtual void release(uint32_t val) = 0;
    virtual void set_range(uint32_t min_val, uint32_t max_val) = 0;
};


id_generator* create_random_id_generator();
id_generator* create_sequential_id_generator();


}
