#pragma once

#include <xul/log/log_configurator.hpp>
#include <xul/log/log_filter_chain.hpp>
#include <xul/lang/object_impl.hpp>

#include <xul/util/options_utils.hpp>
#include <xul/log/logging.hpp>
#include <xul/libs/log/log_service_impl.hpp>
#include <xul/libs/log/asio_log_transmitter.hpp>
#include <xul/libs/log/file_log_appender.hpp>
//#include <xul/log/detail/http_log_appender.hpp>
#include <xul/libs/log/simple_log_appenders.hpp>
#ifdef ANDROID
#include <xul/libs/log/android_log_appender.hpp>
#endif
#include <xul/os/file_system.hpp>
#include <xul/data/numerics.hpp>
#include <xul/io/stdfile.hpp>
#include <xul/std/strings.hpp>
#include <xul/std/maps.hpp>
#include <xul/macro/foreach.hpp>
#include <xul/macro/iterate.hpp>


namespace xul {


class properties_log_configurator : public object_impl<log_configurator>
{
public:
    properties_log_configurator() : m_last_configure_time(0)
    {
    }
    virtual log_filter_chain* configure_filter_chain(const char* filepath)
    {
        if (NULL == filepath)
            return NULL;
        boost::intrusive_ptr<options> opts = create_dynamic_options();
        options_utils::parse_conf(opts.get(), filepath);
        return configure_filter_chain(opts.get());
    }
    virtual log_filter_chain* configure_filter_chain(options* opts)
    {
        if (m_last_configure_time.elapsed() < 5000)
            return NULL;
        log_filter_chain* filters = create_log_filter_chain();
        std::string line;
        typedef std::map<std::string, string_dict> filters_prop_collection;
        std::vector<std::string> filters_names;
        option* filters_opt = opts->get_option("filters");
        if  (filters_opt)
        {
            std::string filters_str = filters_opt->get_value()->get_string("");
            strings::split(std::back_inserter(filters_names), filters_str, ',');
            for (size_t i = 0; i < filters_names.size(); ++i)
            {
                strings::trim(filters_names[i]);
            }
        }
        options* filter_opts = opts->get_options("filter");
        bool filters_added = false;
        for (size_t i = 0; i < filters_names.size(); ++i)
        {
            std::string filter_type = filters_names[i];
            boost::intrusive_ptr<log_filter> filter = create_log_filter(filter_type.c_str());
            if (filter)
            {
                filters_added = true;
                options* filter_sub_opts = filter_opts ? filter_opts->get_options(filter_type.c_str()) : NULL;
                if (filter_sub_opts)
                {
                    XUL_ITERATE(options::option_item_map, filter_sub_opts->get_items(), iter)
                    {
                        const options::option_item_map::entry_type* entry = iter->element();
                        const option* opt = entry->get_value();
                        filter->set_parameter(entry->get_key(), opt->get_value()->get_string(""));
                    }
                }
                filters->add_filter(filter.get());
            }
        }
        return filters;
    }
    virtual bool configure(log_service* service, const char* filepath)
    {
        if (NULL == filepath)
            return false;
        std::string config_filepath = filepath;
        if (config_filepath.empty())
            return false;
        stdfile_reader fin;
        if (false == fin.open_text(config_filepath.c_str()))
            return false;
        std::string line;
        typedef std::map<std::string, string_dict> filters_prop_collection;
        filters_prop_collection filters_props;
        std::vector<std::string> filters_names;
        string_dict loggers_levels;
        for (int line_count = 0; line_count < 2000 && fin.read_line(line); ++line_count)
        {
            strings::trim_spaces(line);
            std::pair<std::string, std::string> key_value = strings::split_pair(line, '=');
            if (key_value.first.empty())
                continue;
            std::vector<std::string> keys;
            strings::split(std::back_inserter(keys), key_value.first, '.', 6);
            if (keys.empty())
                continue;
            for (size_t i = 0; i < keys.size(); ++i)
            {
                strings::trim(keys[i]);
            }
            if (keys[0] == "filters")
            {
                // list all filters
                if (keys.size() == 1)
                {
                    strings::split(std::back_inserter(filters_names), key_value.second, ',');
                    for (size_t i = 0; i < filters_names.size(); ++i)
                    {
                        strings::trim(filters_names[i]);
                        filters_props[filters_names[i]][""] = filters_names[i];
                    }
                }
            }
            else if (keys[0] == "filter")
            {
                // set filter property
                if (keys.size() > 1 && false == keys[1].empty())
                {
                    string_dict& props = filters_props[keys[1]];
                    if (keys.size() == 2)
                        props[""] = key_value.second;
                    else if (keys.size() == 3 && false == keys[2].empty())
                        props[keys[2]] = key_value.second;
                }
            }
            else if (keys[0] == "logger")
            {
                // set logger property
                if (keys.size() > 1 && false == keys[1].empty())
                {
                    loggers_levels[keys[1]] = key_value.second;
                }
            }
            else if (keys[0] == "service")
            {
                // set logger property
                if (keys.size() == 2 && keys[1] == "level")
                {
                    int level = 0;
                    if (log_levels::try_parse(key_value.second, level))
                        service->set_log_level(level);
                }
            }
        }
        XUL_FOREACH_CONST(string_dict, loggers_levels, iter)
        {
            logger* a_logger = service->get_logger(iter->first.c_str());
            int level = 0;
            if (log_levels::try_parse(iter->second, level))
            {
                a_logger->set_level(level);
            }
        }
        bool filters_added = false;
        for (size_t i = 0; i < filters_names.size(); ++i)
        {
            string_dict& props = filters_props[filters_names[i]];
            std::string filter_type = props[""];
            boost::intrusive_ptr<log_filter> filter;
            if (filter_type == "console")
                filter.reset(new console_log_appender);
            else if (filter_type == "asio")
                filter.reset(new asio_log_transmitter);
            else if (filter_type == "file")
                filter.reset(new file_log_appender);
#ifdef ANDROID
            else if (filter_type == "android")
            {
                //printf("create android log appender\n");
                filter.reset(new android_log_appender);
            }
#endif
#if 0
            else if (filter_type == "http")
                filter.reset(new http_log_appender);
#endif
            if (filter)
            {
                filters_added = true;
                XUL_FOREACH_CONST(string_dict, props, iter)
                {
                    if (iter->first.empty())
                        continue;
                    filter->set_parameter(iter->first.c_str(), iter->second.c_str());
                }
                service->add_filter(filter.get());
            }
        }
        return true;
    }
    
private:
    xul::time_counter m_last_configure_time;
};


}
