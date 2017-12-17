#pragma once


namespace xul {


class timer;


class timer_listener
{
public:
    virtual void on_timer_elapsed(timer* sender) = 0;
};


}
