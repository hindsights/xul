#pragma once

#include <xul/util/timer_listener.hpp>
#include <xul/lang/object.hpp>
#include <stdint.h>


namespace boost {
    template <typename R>
    class function0;
}


namespace xul {


class timer_listener;

class timer : public object
{
public:
    virtual void set_listener(timer_listener* listener) = 0;
    virtual void reset_listener() = 0;
    //virtual void set_callback(const boost::function0<void>& callback) = 0;
    virtual void reset_callback() = 0;

    virtual bool start(int interval) = 0;
    virtual void stop() = 0;
    virtual bool is_started() const = 0;
    virtual int get_interval() const = 0;
    virtual int64_t get_times() const = 0;
    virtual int64_t get_elapsed_time() const = 0;
};


}
