#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class logger : public object
{
public:
    virtual const char* get_name() const = 0;

    virtual bool need_log(int level) const = 0;

    virtual void log(const void* context, int level, const char* msg, int len) = 0;

    virtual void log(const void* context, int level, const char* msg) = 0;

    virtual int get_level() const = 0;

    virtual void set_level(int level) = 0;
};


logger* create_dummy_logger(const char* name);


}
