#pragma once

/*
 * @brief functions for asio endpoint
 */

#include <xul/net/inet_socket_address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>


namespace xul {


class endpoints
{
public:
    static inet_socket_address to_inet_socket_address(const boost::asio::ip::tcp::endpoint& ep)
    {
        inet_socket_address addr;
        return to_socket_address(ep, addr) ? addr : inet_socket_address();
    }
    static inet_socket_address to_inet_socket_address(const boost::asio::ip::udp::endpoint& ep)
    {
        inet_socket_address addr;
        return to_socket_address(ep, addr) ? addr : inet_socket_address();
    }

    static bool to_socket_address(const boost::asio::ip::tcp::endpoint& ep, socket_address& addr)
    {
        if (static_cast<size_t>(addr.get_length()) > ep.size())
            return false;
        *addr.get_data() = *ep.data();
        return true;
    }

    static bool to_socket_address(const boost::asio::ip::udp::endpoint& ep, socket_address& addr)
    {
        if (static_cast<size_t>(addr.get_length()) > ep.size())
            return false;
        *addr.get_data() = *ep.data();
        return true;
    }

    static boost::asio::ip::tcp::endpoint to_tcp_endpoint(const socket_address& addr)
    {
        boost::asio::ip::tcp::endpoint ep;
        if ((addr.get_data()->sa_family == AF_INET || addr.get_data()->sa_family == AF_INET6) && (static_cast<size_t>(addr.get_length()) <= ep.capacity()))
        {
            *ep.data() = *addr.get_data();
        }
        else
        {
            assert(false);
        }
        return ep;
    }
    static boost::asio::ip::udp::endpoint to_udp_endpoint(const socket_address& addr)
    {
        boost::asio::ip::udp::endpoint ep;
        if ((addr.get_data()->sa_family == AF_INET || addr.get_data()->sa_family == AF_INET6) && (static_cast<size_t>(addr.get_length()) <= ep.capacity()))
        {
            *ep.data() = *addr.get_data();
        }
        else
        {
            assert(false);
        }
        return ep;
    }
};


}



