#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class log_filter;


class log_filter_chain : public object
{
public:
    virtual bool is_empty() const = 0;
    virtual void add_filter(log_filter* filter) = 0;
    virtual void log(int level, const char* msg, int len) = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;
};


log_filter_chain* create_log_filter_chain();


}
