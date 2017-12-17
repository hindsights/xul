#pragma once

#include <xul/util/random.hpp>
#include <xul/util/time_counter.hpp>
#include <xul/net/asio/io_service_provider.hpp>
#include <xul/util/trace.hpp>

#include <boost/noncopyable.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/asio/deadline_timer.hpp>


namespace xul {


class io_service_runner : private boost::noncopyable
{
public:
    typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;

    io_service_runner() : m_continue(false)
    {

    }

    void use(io_service_ptr ioservice)
    {
        assert(ioservice);
        m_io_service = ioservice;
    }
    void use_default()
    {
        use(xul::io_service_provider::default_instance().get());
    }
    void use_new()
    {
        m_io_service.reset(new boost::asio::io_service);
    }

    boost::shared_ptr<boost::asio::io_service> get_io_service()
    {
        return m_io_service;
    }

    void start()
    {
        m_continue = true;
        m_work.reset(new boost::asio::io_service::work(*m_io_service));;
        m_thread.reset(new boost::thread(boost::bind(&io_service_runner::do_run, this)));
    }
    void stop(unsigned long milliseconds)
    {
        m_continue = false;
        if (m_thread)
        {
            interrupt();
            m_thread->timed_join(boost::posix_time::milliseconds(milliseconds));
            m_thread.reset();
        }
    }
    void stop()
    {
        m_continue = false;
        if (m_thread)
        {
            interrupt();
            m_thread->join();
            m_thread.reset();
        }
    }
    void interrupt()
    {
        m_work.reset();
        m_io_service->stop();
    }
    void wait()
    {
        if (m_thread)
        {
            m_thread->join();
        }
    }
    void run()
    {
        do_run();
    }

private:
    void do_run()
    {
        xul::random::init_seed(xul::time_counter::get_system_count());
        //for (;;)
        //{
        //    m_io_service->run(m_last_error);
        //}

        //eh_initializer();
        XULTRACE("run io_service");

        bool continueLoop = true;
        while (continueLoop)
        {
            m_io_service->run(m_last_error);
            if (m_last_error)
            {
                XULTRACE("error occurred when running io_service. code=%d message=%s", m_last_error.value(), m_last_error.message().c_str());
            }
            m_io_service->reset(); // reset internal state of the io_service, prepare for running continuously
            continueLoop = m_continue;
        }
        XULTRACE("quit io_service");
    }

private:
    io_service_ptr m_io_service;
    boost::shared_ptr<boost::thread> m_thread;
    boost::shared_ptr<boost::asio::io_service::work> m_work;
    boost::system::error_code m_last_error;
    bool m_continue;
};


}
