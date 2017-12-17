#pragma once

#include <xul/util/options.hpp>
#include <xul/log/log_config.hpp>
#include <xul/log/log_service.hpp>
#include <xul/log/log_filter.hpp>
#include <xul/log/log_filters.hpp>
#include <xul/log/log_filter_chain.hpp>
#include <xul/log/logging.hpp>
#include <xul/log/log_levels.hpp>
#include <xul/log/log_configurator.hpp>
#include <xul/lib/library_manager.hpp>
#include <xul/libs/log/asio_log_transmitter.hpp>
#include <xul/libs/log/file_log_appender.hpp>
//#include <xul/log/detail/http_log_appender.hpp>
#include <xul/libs/log/simple_log_appenders.hpp>
#ifdef ANDROID
#include <xul/libs/log/android_log_appender.hpp>
#endif
#include <xul/util/options_proxy.hpp>
#include <xul/lang/object_ptr.hpp>
#include <xul/data/numerics.hpp>
#include <xul/macro/foreach.hpp>
#include <assert.h>


namespace xul {


class logger;
class log_filter;


class log_manager
{
public:
#ifdef ANDROID
    static bool add_android_logcat_appender(const char* tag)
    {
        assert(logging::get_default_service());
        boost::intrusive_ptr<log_filter> filter = create_android_log_appender();
        if (!filter)
        {
            assert(false);
            return false;
        }
        filter->set_parameter("tag", tag);
        logging::get_default_service()->add_filter(filter.get());
        return true;
    }
#endif
    static bool add_console_appender()
    {
        assert(logging::get_default_service());
        boost::intrusive_ptr<log_filter> filter = create_console_log_appender();
        if (!filter)
        {
            assert(false);
            return false;
        }
        logging::get_default_service()->add_filter(filter.get());
        return true;
    }
    static bool add_asio_transmitter()
    {
        assert(logging::get_default_service());
        boost::intrusive_ptr<log_filter> filter = create_asio_log_transmitter();
        if (!filter)
        {
            assert(false);
            return false;
        }
        logging::get_default_service()->add_filter(filter.get());
        return true;
    }
    static bool add_file_appender(const char* base_dir, const char* name, int roll_size, int roll_count, bool append)
    {
        assert(logging::get_default_service());
        boost::intrusive_ptr<log_filter> filter = create_file_log_appender();
        if (!filter)
        {
            assert(false);
            return false;
        }
        filter->set_parameter("dir", base_dir);
        filter->set_parameter("name", name);
        filter->set_parameter("roll_size", numerics::format(roll_size).c_str());
        filter->set_parameter("roll_count", numerics::format(roll_count).c_str());
        filter->set_parameter("append", append ? "true" : "false");
        logging::get_default_service()->add_filter(filter.get());
        return true;
    }
    static bool start_file_log_service(const std::string& name, int level = LOG_DEBUG)
    {
        boost::intrusive_ptr<log_config> config = create_log_config();
        config->name = name;
        config->enable_console_log = true;
        config->enable_file_log = true;
        config->file_append = false;
        config->log_level = level;
        return start_log_service(config.get());
    }
    static bool start_console_log_service(const std::string& name, int level = LOG_DEBUG)
    {
        boost::intrusive_ptr<log_config> config = create_log_config();
        config->name = name;
        config->enable_console_log = true;
        config->enable_file_log = false;
        config->log_level = level;
        return start_log_service(config.get());
    }

    static bool start_log_service(const log_config* config)
    {
        xul::logging::ref_default_service().reset(create_log_service());
        if (!xul::logging::get_default_service())
        {
            assert(false);
            return false;
        }
        boost::intrusive_ptr<xul::log_configurator> configurator = create_properties_log_configurator();
        if (!configurator || !configurator->configure(xul::logging::get_default_service(), config->config_file.c_str()))
        {
            assert(!config->name.empty());
            if (config->enable_console_log)
                xul::log_manager::add_console_appender();
#ifdef ANDROID
            if (config->enable_android_log)
                xul::log_manager::add_android_logcat_appender(config->name.c_str());
#endif
            if (config->enable_file_log)
            {
                xul::log_manager::add_file_appender(config->base_directory.c_str(), config->name.c_str(), config->file_roll_size, config->file_roll_count, config->file_append);
            }

            if (config->log_level > 0)
            {
                xul::logging::get_default_service()->set_log_level(config->log_level);
            }
        }
        return xul::logging::get_default_service()->start();
    }
    static bool start_log_service(options* opts)
    {
        if (!opts)
            return false;
        xul::logging::ref_default_service().reset(create_log_service());
        if (!xul::logging::get_default_service())
        {
            assert(false);
            return false;
        }
        const option* opt = opts->get_option("config_file");
        std::string config_file = opt ? opt->get_value()->get_string("log.conf") : "log.conf";
        boost::intrusive_ptr<xul::log_configurator> configurator = create_properties_log_configurator();
        if (!configurator || !configurator->configure(xul::logging::get_default_service(), config_file.c_str()))
        {
            configure(opts);
        }
        return xul::logging::get_default_service()->start();
    }

    static bool configure_filter_chain(options* opts)
    {
        if (!opts)
            return false;
        log_service* svc = xul::logging::get_default_service();
        if (!svc)
        {
            assert(false);
            return false;
        }
        boost::intrusive_ptr<log_configurator> configurator = create_properties_log_configurator();
        boost::intrusive_ptr<log_filter_chain> chain = configurator->configure_filter_chain(opts);
        svc->set_filter_chain(chain.get());
        return true;
    }
    static bool configure(options* opts)
    {
        xul::log_manager::configure_filter_chain(opts);
        log_service* svc = xul::logging::get_default_service();
        if (!svc)
        {
            assert(false);
            return false;
        }
        option* level_opt = opts->get_option("level");
        if (level_opt)
        {
            std::string level_str = level_opt->get_value()->get_string("");
            int level = xul::numerics::parse<int>(level_str, -1);
            if (level < 0)
            {
                level = xul::log_levels::parse(level_str, xul::LOG_NOT_SET);
            }
            if (level > 0)
            {
                svc->set_log_level(level);
            }
        }
        return true;
    }
};


}
