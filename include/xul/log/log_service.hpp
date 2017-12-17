#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class logger;
class log_filter;
class log_filter_chain;


class log_service : public object
{
public:
    virtual bool start() = 0;

    virtual void stop() = 0;

    virtual void add_filter(log_filter* filter) = 0;

    virtual void set_log_level(int level) = 0;
    virtual int get_log_level() const = 0;

    virtual logger* get_logger(const char* name) = 0;

    virtual bool need_log(const logger& a_logger, int level) const = 0;

    virtual void log(logger& a_logger, const void* context, int level, const char* msg, int len) = 0;

    virtual void set_filter_chain(log_filter_chain* filters) = 0;
};


log_service* create_log_service();


}
