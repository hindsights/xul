#pragma once

#include <xul/log/logger.hpp>
#include <xul/log/log_level.hpp>
#include <xul/log/log_config.hpp>
#include <xul/util/options_wrapper.hpp>
#include <xul/util/options_proxy.hpp>
#include <xul/lang/object_impl.hpp>
#include <string>
#include <string.h>


namespace xul {


class logger_impl : public object_impl<logger>
{
public:
    explicit logger_impl(log_service* service, const std::string& name) : m_service(service), m_name(name), m_level(0)
    {
    }

    virtual const char* get_name() const { return m_name.c_str(); }

    virtual bool need_log(int level) const
    {
        return m_service->need_log(*this, level);
    }

    virtual void log(const void* context, int level, const char* msg, int len)
    {
        //return;
        if (len > 512*1024)
        {
            //std::string s(msg, 128);
            //printf("log too long %d %p %s\n", len, msg, s.c_str());
            return;
        }
        m_service->log(*this, context, level, msg, len);
    }

    virtual void log(const void* context, int level, const char* msg)
    {
        int len = strlen(msg);
        m_service->log(*this, context, level, msg, len);
    }

    virtual void set_level(int level)
    {
        m_level = level;
    }

    virtual int get_level() const
    {
        if (m_level <= 0)
            return m_service->get_log_level();
        return m_level;
    }

private:
    boost::intrusive_ptr<log_service> m_service;

    std::string m_name;

    int m_level;
};


class dummy_logger_impl : public object_impl<logger>
{
public:
    explicit dummy_logger_impl(const char* name) : m_name(name ? name : "")
    {
    }

    virtual const char* get_name() const { return m_name.c_str(); }

    virtual bool need_log(int level) const
    {
        return false;
    }

    virtual void log(const void* context, int level, const char* msg, int len)
    {
    }

    virtual void log(const void* context, int level, const char* msg)
    {
    }

    virtual void set_level(int level)
    {
    }

    virtual int get_level() const
    {
        return LOG_OFF;
    }

private:
    std::string m_name;
};


logger* create_dummy_logger(const char* name)
{
    return new dummy_logger_impl(name);
}


class log_config_impl : public xul::options_proxy<log_config>
{
public:
    log_config_impl()
    {
        options_wrapper opts(*this);
        opts.add("base_directory", &base_directory, ".");
        opts.add("config_file", &config_file, "log.conf");
#if defined(_DEBUG) || !defined(NDEBUG)
        opts.add("file_roll_size", &file_roll_size, 100 * 1024 * 1024); // 100M
        opts.add_switch("enable_console_log", &enable_console_log, true);
        opts.add_switch("enable_android_log", &enable_android_log, true);
        opts.add("log_level", &log_level, (int)LOG_DEBUG);
#else
        opts.add("file_roll_size", &file_roll_size, 20 * 1024 * 1024); // 100M
        opts.add_switch("enable_console_log", &enable_console_log, false);
        opts.add_switch("enable_file_log", &enable_file_log, false);
        opts.add_switch("enable_android_log", &enable_android_log, false);
        opts.add("log_level", &log_level, (int)LOG_EVENT);
#endif
        opts.add("file_roll_count", &file_roll_count, 2);
        opts.add_switch("file_append", &file_append, false);
    }
};


log_config* create_log_config()
{
    return new log_config_impl;
}


}
