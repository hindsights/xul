#pragma once

#include <boost/config.hpp>

#include <string>

#if defined(BOOST_WINDOWS)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


namespace xul {

/// represents ip address for ipv4
class inet4_address
{
public:
    inet4_address()
    {
        set_raw_address(0);
    }
    void set_address(unsigned long ip)
    {
        m_data.s_addr = htonl(ip);
    }

    void set_address(const char* ipstr)
    {
        set_raw_address(inet_addr(ipstr));
    }

    void set_raw_address(unsigned long ip)
    {
        m_data.s_addr = ip;
    }

    bool is_none() const { return get_address() == INADDR_NONE; }
    bool is_loopback() const { return get_address() == INADDR_LOOPBACK; }
    bool is_valid() const { return !is_none() && get_raw_address() != 0; }

    unsigned long get_address() const
    {
        return ntohl(m_data.s_addr);
    }

    unsigned long get_raw_address() const
    {
        return m_data.s_addr;
    }

    std::string str() const
    {
        return std::string(inet_ntoa(m_data));
    }

    in_addr& get_data() { return m_data; }
    const in_addr& get_data() const { return m_data; }

private:
    in_addr m_data;
};

inline inet4_address make_raw_inet4_address(uint32_t ip)
{
    inet4_address addr;
    addr.set_address(ip);
    return addr;
}
inline inet4_address make_inet4_address(uint32_t ip)
{
    inet4_address addr;
    addr.set_address(ip);
    return addr;
}

inline inet4_address make_inet4_address(const char* ipstr)
{
    inet4_address addr;
    addr.set_address(ipstr);
    return addr;
}

inline std::ostream& operator<<(std::ostream& os, const inet4_address& addr)
{
    return os << addr.str();
}

}


