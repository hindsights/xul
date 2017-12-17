#pragma once

#include <xul/libs/log/log_filter_impl.hpp>
#include <xul/net/asio/io_service_runner.hpp>
#include <xul/io/stdfile.hpp>
#include <xul/util/time_counter.hpp>
#include <xul/util/data_parser.hpp>
#include <xul/os/file_system.hpp>
#include <boost/shared_ptr.hpp>


namespace xul {


class file_log_appender : public log_filter_impl
{
public:
    class log_message
    {
    public:
        std::string text;
    };
    
    file_log_appender() : m_dirty(false), m_is_append_mode(false)
    {
        m_param_parser.add("dir", &m_dir);
        m_param_parser.add("name", &m_name);
        m_param_parser.add_binary_byte_count<size_t>("roll_size", &m_roll_size, 0);
        m_param_parser.add("roll_count", &m_roll_count, 0);
        m_param_parser.add("append", &m_is_append_mode, false);
        m_param_parser.add("append", &m_is_append_mode, false);
        m_param_parser.add("background", &m_background, true);
    }
    ~file_log_appender()
    {
    }

    virtual void set_parameter(const char* name, const char* val)
    {
        m_param_parser.parse(name, val);
    }

    virtual void log(int level, const char* msg, int len)
    {
        if (m_background)
        {
            //boost::shared_ptr<std::string> msgstr(new std::string(msg, len));
            boost::shared_ptr<log_message> logmsg(new log_message);
            //boost::intrusive_ptr<log_message> logmsg(new log_message);
            logmsg->text.assign(msg, len);
            m_io_runner->get_io_service()->post(boost::bind(&file_log_appender::on_log, boost::intrusive_ptr<file_log_appender>(this), level, logmsg));
            //m_io_runner->get_io_service()->post(boost::bind(&asio_log_transmitter::on_log_test, this, level));
            return;
        }
        do_log(level, msg, len);
    }
    
    void do_log(int level, const char* msg, int len)
    {
        if (m_fout.is_open())
        {
            m_fout.write(msg, len);
            m_fout.write("\n");
            if ( m_last_flush_time.get_elapsed32() > 5 * 1000 )
            {
                flush_log();
            }
        }
        invoke_next(level, msg, len);
    }

    virtual bool start()
    {
        if (m_dir.empty())
            m_dir = file_system::current_directory();
        if (m_name.empty())
            return false;
        if (!open_log(true))
            return false;
        m_abort = false;
        if (m_background)
        {
            if (!m_io_runner)
            {
                m_io_runner.reset(new xul::io_service_runner);
                m_io_runner->use_new();
            }
            m_io_runner->start();
        }
        return true;
    }

    virtual void stop()
    {
        if (m_background)
        {
            m_abort = true;
            if (m_io_runner)
            {
                m_io_runner->get_io_service()->post(boost::bind(&file_log_appender::on_stop, boost::intrusive_ptr<file_log_appender>(this)));
                m_io_runner->stop(2000);
            }
        }
        else
        {
            m_fout.close();
        }
    }

private:
    bool open_log(bool first_time)
    {
        m_dirty = false;
        std::string filename = m_name + ".log";
        if (false == xul::file_system::ensure_directory_exists(m_dir.c_str()))
            return false;
        std::string filepath = xul::paths::combine( m_dir, filename );
        m_fout.close();
        bool open_result;
        if (first_time && m_is_append_mode)
            open_result = m_fout.open_binary_appending(filepath.c_str());
        else
            open_result = m_fout.open_binary(filepath.c_str());
        if ( false == open_result )
        {
            printf("failed to open log file %s\n", filepath.c_str());
            //assert(false);
            return false;
        }
        if (first_time && m_is_append_mode)
        {
            m_fout.write("\n\n");
        }
        return true;
    }

    void flush_log()
    {
        m_dirty = false;
        m_fout.flush();
        m_last_flush_time.sync();
        //printf("sync flush log\n");
        if ( m_roll_size > 0 && m_fout.get_size() > m_roll_size )
        {
            if (m_roll_count > 0)
            {
                roll_over();
            }
            this->open_log(false);
        }
    }

    void roll_over()
    {
        m_fout.close();
        if (m_roll_count <= 0)
            return;
        std::string filepath = paths::combine(m_dir, m_name + strings::format(".%d.log", m_roll_count - 1));
        file_system::remove_file(filepath.c_str());
        for (int index = m_roll_count - 2; index >= 0; --index)
        {
            std::string young_filepath = paths::combine(m_dir, m_name + strings::format(".%d.log", index));
            file_system::rename(young_filepath.c_str(), filepath.c_str());
            filepath = young_filepath;
        }
        std::string young_filepath = paths::combine(m_dir, m_name + ".log");
        file_system::rename(young_filepath.c_str(), filepath.c_str());
    }

private:
    void on_log(int level, const boost::shared_ptr<log_message>& msg)
    {
        if (m_abort)
            return;
        //printf("on_log %p %p %d\n", msg.get(), msg->c_str(), msg->size());
        do_log(level, msg->text.c_str(), msg->text.size());
        //invoke_next("12345", 5);
    }
    void on_stop()
    {
        m_fout.close();
    }
    void on_log_test(int level)
    {
        if (m_abort)
            return;
        //printf("on_log %p %p %d\n", msg.get(), msg->c_str(), msg->size());
        //invoke_next(msg->text.c_str(), msg->text.size());
        invoke_next(level, "12345", 5);
    }

private:
    stdfile_writer m_fout;
    std::string m_dir;
    std::string m_name;

    time_counter m_last_flush_time;
    bool m_dirty;
    bool m_is_append_mode;
    bool m_background;

    size_t m_roll_size;

    int m_roll_count;

    data_parser m_param_parser;
    boost::shared_ptr<io_service_runner> m_io_runner;
    bool m_abort;
};


}
