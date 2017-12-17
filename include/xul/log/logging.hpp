#pragma once

#include <xul/log/logger.hpp>
#include <xul/log/log_service.hpp>
#include <xul/lang/object_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <string>


namespace xul {


class logging
{
public:
    static logger* get_logger(const char* name)
    {
        return get_default_service() ? get_default_service()->get_logger(name) : get_null_logger();
    }
    static logger* get_null_logger()
    {
        static boost::intrusive_ptr<logger> null_logger = create_dummy_logger("null");
        return null_logger.get();
    }

    static boost::intrusive_ptr<log_service>& ref_default_service()
    {
        static boost::intrusive_ptr<log_service> the_service;
        return the_service;
    }

    static log_service* get_default_service()
    {
        return ref_default_service().get();
    }

    static void start_service(log_service* instance)
    {
        ref_default_service().reset(instance);
        get_default_service()->start();
    }

    static void set_service(log_service* instance)
    {
        ref_default_service().reset(instance);
    }

    static logger* get_libxul_logger()
    {
        static logger* the_libxul_logger = get_logger("xul");
        return the_libxul_logger;
    }

    static logger* get_app_logger()
    {
        static logger* the_app_logger = get_logger("app");
        return the_app_logger;
    }

    class service_stopper : private boost::noncopyable
    {
    public:
        ~service_stopper()
        {
            logging::get_default_service()->stop();
        }
    };
};


}

#define XUL_REL_LOGGER_INIT(name)        m_xul_logger = xul::logging::get_logger(name)
#define XUL_LOGGER_INIT(name)            XUL_REL_LOGGER_INIT(name)

#define XUL_LOGGER_DEFINE_STATIC(name)    \
    static xul::logger* get_xul_logger()\
    {\
        static boost::intrusive_ptr<xul::logger> s_xul_logger = xul::logging::get_logger(name);\
        return s_xul_logger.get();\
    }\
    static const void* get_xul_logger_owner() { return 0; }
