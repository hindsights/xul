#pragma once

#include <xul/util/timer.hpp>
#include <assert.h>


namespace xul {
namespace detail {


class dummy_timer_listener : public timer_listener
{
public:
    virtual void on_timer_elapsed(timer* sender) { }
};

class checked_timer_listener : public timer_listener
{
public:
    virtual void on_timer_elapsed(timer* sender) { assert(false); }
};


}
}
