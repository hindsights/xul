#pragma once

#include <xul/config.hpp>
#include <xul/net/io_service.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/net/asio/io_service_runner.hpp>
#include <xul/libs/asio/asio_name_resolver.hpp>
#include <xul/libs/asio/asio_tcp_socket.hpp>
#include <xul/libs/asio/asio_tcp_acceptor.hpp>
#include <xul/libs/asio/asio_udp_socket.hpp>
#include <xul/libs/asio/asio_timer.hpp>
#include <xul/log/log.hpp>
#if !defined(XUL_WINDOWS)
//#include <xul/libs/asio/asio_message_socket.hpp>
#endif
#include <xul/net/asio/io_service_provider.hpp>
#include <xul/util/runnable.hpp>
#include <xul/log/log.hpp>
#include <boost/bind.hpp>


namespace xul {


class asio_service : public object_impl<io_service>
{
public:
    asio_service()
    {
        XUL_LOGGER_INIT("asio_service");
        XUL_DEBUG("new");
        m_runner.use_new();
    }
    ~asio_service()
    {
        XUL_DEBUG("delete");
    }

    boost::shared_ptr<boost::asio::io_service> get_native_service()
    {
        return m_runner.get_io_service();
    }

    virtual void start()
    {
        XUL_DEBUG("start");
        m_runner.start();
    }
    virtual void stop()
    {
        XUL_DEBUG("stop");
        m_runner.stop();
    }
    virtual void stop(unsigned long milliseconds)
    {
        XUL_DEBUG("stop " << milliseconds);
        m_runner.stop(milliseconds);
    }
    virtual void run()
    {
        m_runner.run();
    }
    virtual void wait()
    {
        XUL_DEBUG("wait");
        m_runner.wait();
    }
    virtual void interrupt()
    {
        XUL_DEBUG("interrupt");
        m_runner.interrupt();
    }
    virtual void post(runnable* r)
    {
        XUL_DEBUG("post " << r);
        m_runner.get_io_service()->post(boost::bind(&runnable::run, boost::intrusive_ptr<runnable>(r)));
    }
    virtual name_resolver* create_name_resolver()
    {
        return new asio_name_resolver(*m_runner.get_io_service());
    }
    virtual udp_socket* create_udp_socket()
    {
        return new asio_udp_socket(*m_runner.get_io_service());
    }
    virtual tcp_socket* create_tcp_socket()
    {
        return new asio_tcp_socket(*m_runner.get_io_service());
    }
    virtual tcp_acceptor* create_tcp_acceptor()
    {
        return new asio_tcp_acceptor(*m_runner.get_io_service());
    }
    virtual timer* create_once_timer()
    {
        return new asio_once_timer(*m_runner.get_io_service());
    }
    virtual timer* create_periodic_timer()
    {
        return new asio_periodic_timer(*m_runner.get_io_service());
    }
    virtual void* get_native()
    {
        return m_runner.get_io_service().get();
    }

    virtual udp_socket* recreate_udp_socket(int handle)
    {
        return new asio_udp_socket(*m_runner.get_io_service(), handle);
    }

    virtual tcp_socket* recreate_tcp_socket(int handle, bool connected)
    {
        XUL_APP_REL_DEBUG("recreate_tcp_socket " << handle << " " << m_runner.get_io_service());
        return new asio_tcp_socket(*m_runner.get_io_service(), handle);
    }

    virtual tcp_acceptor* recreate_tcp_acceptor(int handle)
    {
        return new asio_tcp_acceptor(*m_runner.get_io_service(), handle);
    }
#if defined(XUL_WINDOWS)
    virtual message_socket* recreate_message_socket(int handle)
    {
        assert(false);
        return NULL;
    }
#else
    virtual message_socket* recreate_message_socket(int handle)
    {
        //return new asio_message_socket(*m_runner.get_io_service(), handle);
        return NULL;
    }
#endif

private:
    XUL_LOGGER_DEFINE();
    io_service_runner m_runner;
};


}
