#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>


namespace xul {


class io_service_provider : private boost::noncopyable
{
public:
    typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;

    io_service_provider() : m_io_service(new boost::asio::io_service)
    {
    }
    explicit io_service_provider(const io_service_ptr& p) : m_io_service(p)
    {
    }

    io_service_ptr get() { return m_io_service; }

    boost::asio::io_service& get_io_service()
    {
        return *m_io_service;
    }

    void reset()
    {
        m_io_service.reset();
    }

    static io_service_provider& default_instance()
    {
        static io_service_provider provider;
        return provider;
    }
    static boost::asio::io_service& default_io_service()
    {
        return default_instance().get_io_service();
    }

private:
    io_service_ptr m_io_service;
};

}
