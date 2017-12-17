#pragma once

#include <xul/libs/log/log_filter_impl.hpp>
#include <xul/net/asio/io_service_runner.hpp>


namespace xul {


class asio_log_transmitter : public log_filter_impl
{
public:
    class log_message
    {
    public:
        std::string text;
    };
    asio_log_transmitter() : m_abort(true)
    {
        m_io_runner.reset(new io_service_runner);
        m_io_runner->use_new();
    }
    ~asio_log_transmitter()
    {
        this->stop();
    }

    virtual void log(int level, const char* msg, int len)
    {
        //boost::shared_ptr<std::string> msgstr(new std::string(msg, len));
        boost::shared_ptr<log_message> logmsg(new log_message);
        //boost::intrusive_ptr<log_message> logmsg(new log_message);
        logmsg->text.assign(msg, len);
        m_io_runner->get_io_service()->post(boost::bind(&asio_log_transmitter::on_log, boost::intrusive_ptr<asio_log_transmitter>(this), level, logmsg));
        //m_io_runner->get_io_service()->post(boost::bind(&asio_log_transmitter::on_log_test, this, level));
    }

    virtual bool start()
    {
        m_abort = false;
        m_io_runner->start();
        return true;
    }

    virtual void stop()
    {
        m_abort = true;
        m_io_runner->stop(2000);
    }

private:
    void on_log(int level, const boost::shared_ptr<log_message>& msg)
    {
        if (m_abort)
            return;
        //printf("on_log %p %p %d\n", msg.get(), msg->c_str(), msg->size());
        invoke_next(level, msg->text.c_str(), msg->text.size());
        //invoke_next("12345", 5);
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
    boost::shared_ptr<io_service_runner> m_io_runner;
    bool m_abort;
};


}
