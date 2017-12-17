#pragma once

#include <xul/libs/log/log_filter_impl.hpp>
#include <xul/net/asio/io_service_runner.hpp>
#include <xul/util/trace.hpp>


namespace xul {


class console_log_appender : public log_filter_impl
{
public:
    virtual void log(int level, const char* msg, int len)
    {
        printf("%s\n", msg);
        invoke_next(level, msg, len);
    }
};


class trace_log_appender : public log_filter_impl
{
public:
    virtual void log(int level, const char* msg, int len)
    {
        tracer::output(msg);
        tracer::output("\n");
        invoke_next(level, msg, len);
    }
};


class fake_log_appender : public log_filter_impl
{
public:
    virtual void log(int level, const char* msg, int len)
    {
        invoke_next(level, msg, len);
    }
};


class null_log_appender : public log_filter_impl
{
public:
    virtual void log(int level, const char* msg, int len)
    {
    }
};


}
