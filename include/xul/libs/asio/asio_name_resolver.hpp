#pragma once

#include <xul/net/name_resolver.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/util/listenable_mixin.hpp>
#include <xul/net/detail/dummy_name_resolver_listener.hpp>
#include <xul/net/asio/io_service_provider.hpp>
#include <xul/net/inet4_address.hpp>
#include <xul/net/asio/endpoints.hpp>
#include <xul/net/dns_backup.hpp>
#include <xul/util/listenable.hpp>
#include <xul/util/intrusive_shared_from_this.hpp>
#include <xul/log/log.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>


namespace xul {


/// resolve host name
class asio_name_resolver
    : public closable_listenable_mixin<object_impl<name_resolver>, name_resolver_listener, detail::dummy_name_resolver_listener, detail::checked_name_resolver_listener>
    , public intrusive_shared_from_this<asio_name_resolver>
{
private:
    typedef boost::asio::ip::tcp::resolver resolver_type;
public:
    explicit asio_name_resolver(boost::asio::io_service& ios = xul::io_service_provider::default_io_service())
        : m_resolver(ios)
        , m_query(boost::asio::ip::tcp::v4(), "", "")
    {
        XUL_LOGGER_INIT("name_resolver");
        XUL_DEBUG("new");
    }
    ~asio_name_resolver()
    {
        this->destroy();
        XUL_DEBUG("delete");
    }
    virtual bool resolve(const std::string& name, std::vector<inet4_address>& addresses)
    {
        XUL_DEBUG("resolve " << name);
        addresses.clear();
        m_query = resolver_type::query(boost::asio::ip::tcp::v4(), name, "");
        resolver_type::iterator endpoint_iterator = m_resolver.resolve(m_query, m_last_error);
        if (m_last_error)
        {
            XUL_REL_ERROR("resolve failed " << name << " " << m_last_error);
            add_backup_addresses(name, addresses);
            return !addresses.empty();
        }
        for (resolver_type::iterator iter = endpoint_iterator; iter != resolver_type::iterator(); ++iter)
        {
            xul::inet_socket_address addr = xul::endpoints::to_inet_socket_address(*iter);
            addresses.push_back(addr.get_inet_address());
        }
        if (addresses.empty())
        {
            add_backup_addresses(name, addresses);
        }
        return true;
    }
    virtual void async_resolve(const std::string& name)
    {
        m_query = resolver_type::query(boost::asio::ip::tcp::v4(), name, "");
        m_addresses.clear();
        m_resolver.async_resolve(m_query, boost::bind(&asio_name_resolver::handle_resolve, shared_from_this(), _1, _2));
    }
    virtual void close()
    {
        m_resolver.cancel();
        m_addresses.clear();
    }

private:
    void add_backup_addresses(const std::string& name, std::vector<inet4_address>& addresses)
    {
        assert(addresses.empty());
        boost::intrusive_ptr<dns_backup> dnsbak = dns_backup::get_global_backup();
        if (!dnsbak)
            return;
        boost::intrusive_ptr<xul::list<uint32_t> > ipv4_addrs = dnsbak->lookup(name.c_str());
        if (!ipv4_addrs)
            return;
        XUL_REL_WARN("add_backup_addresses " << name << " " << ipv4_addrs->size());
        for (int i = 0; i < ipv4_addrs->size(); ++i)
        {
            inet4_address addr;
            addr.set_raw_address(ipv4_addrs->get(i));
            XUL_REL_WARN("add_backup_addresses " << name << " " << i << " " << addr);
            addresses.push_back(addr);
        }
    }
    void handle_resolve(const boost::system::error_code& err, resolver_type::iterator endpoint_iterator)
    {
        m_addresses.clear();
        if (err)
        {
            add_backup_addresses(m_query.host_name(), m_addresses);
            if (m_addresses.empty())
            {
                do_get_listener()->on_resolver_address(this, m_query.host_name(), err.value(), m_addresses);
            }
            else
            {
                do_get_listener()->on_resolver_address(this, m_query.host_name(), 0, m_addresses);
            }
            return;
        }
        // Attempt a connection to the first endpoint in the list. Each endpoint
        // will be tried until we successfully establish a connection.
        resolver_type::iterator end_iter;
        for (resolver_type::iterator iter = endpoint_iterator; iter != end_iter; ++iter)
        {
            xul::inet_socket_address addr = xul::endpoints::to_inet_socket_address(*iter);
            m_addresses.push_back(addr.get_inet_address());
        }
        if (m_addresses.empty())
            add_backup_addresses(m_query.host_name(), m_addresses);
        if (m_addresses.empty())
        {
            do_get_listener()->on_resolver_address(this, m_query.host_name(), -1, m_addresses);
            return;
        }
        do_get_listener()->on_resolver_address(this, m_query.host_name(), 0, m_addresses);
    }

    void on_resolve()
    {

    }

private:
    XUL_LOGGER_DEFINE();
    resolver_type m_resolver;
    resolver_type::query m_query;
    std::vector<inet4_address> m_addresses;
    boost::system::error_code m_last_error;
};


}
