#pragma once

#include <xul/log/log_service.hpp>
#include <xul/log/log_filter.hpp>
#include <xul/log/log_filter_chain.hpp>
#include <xul/log/log_levels.hpp>
#include <xul/libs/log/logger_impl.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/util/trace.hpp>

#include <xul/io/stdfile.hpp>
#include <xul/util/time_counter.hpp>
#include <xul/data/numerics.hpp>
#include <xul/os/paths.hpp>
#include <xul/os/file_system.hpp>
#include <xul/data/date_time.hpp>
#include <xul/os/thread.hpp>
#include <xul/std/maps.hpp>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/unordered_map.hpp>

#include <fstream>
#include <map>
#include <set>
#include <list>
#include <iomanip>
#include <assert.h>


namespace xul {


class log_service_impl : public object_impl<log_service>
{
public:
    typedef boost::intrusive_ptr<logger> logger_ptr;
    typedef boost::intrusive_ptr<log_filter> log_filter_ptr;
    typedef boost::unordered_map<std::string, logger_ptr> logger_collection;
    typedef boost::shared_ptr<logger_collection> logger_collection_ptr;
    typedef std::map<int, std::string> value_string_table;

    class log_entry : private boost::noncopyable
    {
    public:
        int pid;
        date_time time;
        std::string name;
        int level;
        std::string message;

        log_entry() : pid(0), level(0)
        {
        }
    };

    log_service_impl()
    {
        XULTRACE("log_service.new\n");
        m_loggers.reset(new logger_collection);
#if defined(_DEBUG) || !defined(NDEBUG)
        m_log_level = LOG_DEBUG;
#else
        m_log_level = LOG_INFO;
#endif
        //m_root_logger.reset(new logger(this, "root"));
        m_root_logger = get_logger("root");
        m_started = false;
        m_filters = create_log_filter_chain();
    }
    virtual ~log_service_impl()
    {
        XULTRACE("log_service.delete\n");
        stop();
    }

    virtual bool start()
    {
        if (!start_filters())
            return false;
        m_started = true;
        m_root_logger->log(this, LOG_EVENT, "log service started.");
        return true;
    }
    virtual void stop()
    {
        XULTRACE("log_service.stop\n");
        stop_filters();
        m_started = false;
    }

    virtual bool need_log( const logger& a_logger, int level ) const
    {
        if (m_filters->is_empty())
            return false;
        if (!m_started)
            return false;
        return level >= a_logger.get_level();
    }

    virtual void set_log_level( int level )
    {
        m_log_level = level;
    }
    virtual int get_log_level() const
    {
        return m_log_level;
    }

    virtual logger* get_logger(const char* name)
    {
        logger_collection_ptr loggers = boost::atomic_load(&m_loggers);
        logger_collection::const_iterator iter = loggers->find(name);
        if (iter != loggers->end())
            return iter->second.get();
        logger_collection_ptr new_loggers(new logger_collection(*loggers));
        logger_ptr item(new logger_impl(this, name));
        (*new_loggers)[name] = item;
        boost::atomic_store(&m_loggers, new_loggers);
        return item.get();
    }

    virtual void log( logger& a_logger, const void* context, int level, const char* msg, int len )
    {
        boost::intrusive_ptr<log_filter_chain> filters = m_filters;
        if (filters->is_empty())
            return;
        std::string timestr = date_time::now().str();
        std::string name = a_logger.get_name();
        std::string levelstr = log_levels::get_name(level);
        std::ostringstream os;
        os  << timestr << " " << std::setw(5) << levelstr
            << " " << std::setw(6) << thread::current_thread_id()
            << " " << context
            << std::setw(0) << " [" << a_logger.get_name() << "] " << msg;
        std::string msgstr = os.str();
        filters->log(level, msgstr.c_str(), msgstr.size());
    }
    virtual void add_filter(log_filter* filter)
    {
        m_filters->add_filter(filter);
    }
    bool start_filters()
    {
        return m_filters->start();
    }
    void stop_filters()
    {
        m_filters->stop();
    }
    virtual void set_filter_chain(log_filter_chain* filters)
    {
        if (!filters)
            return;
        stop();
        m_filters = filters;
        start();
    }

protected:
    static std::string get_value_string( int val, const value_string_table& strs )
    {
        value_string_table::const_iterator iter = strs.find( val );
        if ( iter != strs.end() )
            return iter->second;
        return numerics::format<int>( val );
    }

private:
    logger_collection_ptr m_loggers;
    boost::intrusive_ptr<log_filter_chain> m_filters;
    int m_log_level;
    logger_ptr m_root_logger;
    bool m_started;
};



}
