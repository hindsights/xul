#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class log_filter : public object
{
public:
    virtual void log(int level, const char* msg, int len) = 0;

    virtual void set_parameter(const char* name, const char* val) = 0;

    virtual log_filter* get_next() const = 0;

    virtual void set_next(log_filter* filter) = 0;

    virtual bool start() = 0;

    virtual void stop() = 0;
};


log_filter* create_log_filter(const char* type);


}
