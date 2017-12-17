#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class runnable;


class task_service : public object
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void stop(unsigned long milliseconds) = 0;
    virtual void wait() = 0;
    virtual void interrupt() = 0;
    virtual void post(runnable* r) = 0;
    virtual void run() = 0;
    //virtual int get_pending_task_count() const = 0;
};


}
