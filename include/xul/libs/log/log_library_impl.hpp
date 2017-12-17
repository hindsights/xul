
#include <xul/config.hpp>
#include <xul/log/log_library.hpp>
#include <xul/log/log_service.hpp>
#include <xul/libs/log/log_service_impl.hpp>
#include <xul/libs/log/log_filter_chain_impl.hpp>
#include <xul/libs/log/properties_log_configurator.hpp>
#include <xul/lib/library.hpp>
#include <xul/lib/library_repository.hpp>
#include <xul/lib/detail/library_info_impl.hpp>
#include <xul/lang/detail/class_info_impl.hpp>
#include <xul/lang/detail/global_service_manager_impl.hpp>
#include <xul/lang/object_impl.hpp>

#include <xul/lang/object_ptr.hpp>
#include <boost/intrusive_ptr.hpp>


namespace xul {

class log_global_service_manager_impl : public xul::global_service_manager_impl
{
public:
    virtual void set_default_log_service(xul::log_service* svc)
    {
    }
};

class properties_log_configurator_class : public xul::class_info_impl
{
public:
    properties_log_configurator_class() : xul::class_info_impl("xul.log.properties_log_configurator")
    {
    }

    virtual object* create_object()
    {
        return new xul::properties_log_configurator;
    }
};

class log_library_impl : public xul::object_impl<xul::log_library>
{
public:
    log_library_impl()
    {
        m_global_service_manager.reset(new log_global_service_manager_impl);
    }

    virtual xul::global_service_manager* get_service_manager()
    {
        return m_global_service_manager.get();
    }
    virtual xul::log_service* create_service()
    {
        return new xul::log_service_impl;
    }
    virtual xul::log_configurator* create_properties_configurator()
    {
        return new xul::properties_log_configurator;
    }

private:
    boost::intrusive_ptr<xul::global_service_manager> m_global_service_manager;
    boost::intrusive_ptr<xul::log_service> m_log_service;
};


class log_service_class : public xul::class_info_impl
{
public:
    log_service_class() : xul::class_info_impl("xul.log.log_service")
    {
    }

    virtual object* create_object()
    {
        return new xul::log_service_impl;
    }
};

class console_log_appender_class : public xul::class_info_impl
{
public:
    console_log_appender_class() : xul::class_info_impl("xul.log.console_log_appender")
    {
    }

    virtual object* create_object()
    {
        return new xul::console_log_appender;
    }
};

class asio_log_transmitter_class : public xul::class_info_impl
{
public:
    asio_log_transmitter_class() : xul::class_info_impl("xul.log.asio_log_transmitter")
    {
    }

    virtual object* create_object()
    {
        return new xul::asio_log_transmitter;
    }
};

class file_log_appender_class : public xul::class_info_impl
{
public:
    file_log_appender_class() : xul::class_info_impl("xul.log.file_log_appender")
    {
    }

    virtual object* create_object()
    {
        return new xul::file_log_appender;
    }
};

#ifdef ANDROID
class android_log_appender_class : public xul::class_info_impl
{
public:
    android_log_appender_class() : xul::class_info_impl("xul.log.android_log_appender")
    {
    }

    virtual object* create_object()
    {
        return new xul::android_log_appender;
    }
};
#endif

class log_library_class : public xul::class_info_impl
{
public:
    log_library_class() : xul::class_info_impl("xul.log_library")
    {
    }

    virtual object* create_object()
    {
        return new log_library_impl;
    }
};

class log_library_info : public xul::library_info_impl
{
public:
    typedef boost::intrusive_ptr<xul::class_info> class_info_ptr;

    log_library_info() : xul::library_info_impl("xul.log", 0x00000001)
    {
        add_class(new log_library_class);
        add_class(new log_service_class);
        add_class(new properties_log_configurator_class);
        add_class(new console_log_appender_class);
        add_class(new asio_log_transmitter_class);
        add_class(new file_log_appender_class);
#ifdef ANDROID
        add_class(new android_log_appender_class);
#endif
    }

    virtual xul::library* load_library()
    {
        if (!m_log_library)
        {
            m_log_library = new log_library_impl;
        }
        return m_log_library.get();
    }

private:
    boost::intrusive_ptr<xul::library> m_log_library;
};

log_configurator* create_properties_log_configurator()
{
    return new properties_log_configurator;
}


log_service* create_log_service()
{
    return new log_service_impl;
}


log_filter_chain* create_log_filter_chain()
{
    return new log_filter_chain_impl;
}

#ifdef ANDROID
log_filter* create_android_log_appender()
{
    return new android_log_appender;
}
#endif

log_filter* create_console_log_appender()
{
    return new console_log_appender;
}
log_filter* create_asio_log_transmitter()
{
    return new asio_log_transmitter;
}
log_filter* create_file_log_appender()
{
    return new file_log_appender;
}

log_filter* create_log_filter(const char* type)
{
    if (NULL == type)
        return NULL;
    log_filter* filter = NULL;
    std::string filter_type = type;
    if (filter_type.empty())
    {
        return NULL;
    }
    if (filter_type == "console")
        filter = new console_log_appender;
    else if (filter_type == "asio")
        filter = new asio_log_transmitter;
    else if (filter_type == "file")
        filter = new file_log_appender;
#ifdef ANDROID
    else if (filter_type == "android")
    {
        //printf("create android log appender\n");
        filter = new android_log_appender;
    }
#endif
#if 0
    else if (filter_type == "http")
        filter = new http_log_appender;
#endif
    return filter;
}


}
