#pragma once

#include <xul/log/log.hpp>
#include <xul/libs/log/log_filter_impl.hpp>
#include <xul/util/data_parser.hpp>
#include <android/log.h>


namespace xul {


class android_log_appender : public log_filter_impl
{
public:
    android_log_appender()
    {
        m_param_parser.add("tag", &m_tag, "");
    }

    virtual void log(int level, const char* msg, int len)
    {
        switch (level)
        {
            case LOG_FATAL:
                level = ANDROID_LOG_FATAL;
                break;
            case LOG_ERROR:
            case LOG_EVENT:
                level = ANDROID_LOG_ERROR;
                break;
            case LOG_WARNING:
                level = ANDROID_LOG_WARN;
                break;
            case LOG_INFO:
                level = ANDROID_LOG_INFO;
                break;
            case LOG_DEBUG:
            default:
                level = ANDROID_LOG_DEBUG;
                break;
        }

        __android_log_write(level, m_tag.c_str(), msg);
        invoke_next(level, msg, len);
    }

    virtual void set_parameter(const char* name, const char* val)
    {
        m_param_parser.parse(name, val);
    }

private:
    data_parser m_param_parser;
    std::string m_tag;
};


}
