#pragma once

#include <xul/net/socket_address.hpp>
#include <xul/net/inet4_address.hpp>
#include <xul/macro/fill_zero.hpp>
#include <xul/data/numerics.hpp>
#include <xul/io/output_stream.hpp>

#include <ostream>
#include <sstream>
#include <stdint.h>


namespace xul {

class inet_socket_address : public socket_address
{
public:
    inet_socket_address()
    {
        clear();
    }
    inet_socket_address(uint32_t ip, uint16_t port)
    {
        clear();
        set_ip(ip);
        set_port(port);
    }
    inet_socket_address(const char* ip, uint16_t port)
    {
        clear();
        set_raw_ip(inet_addr(ip));
        set_port(port);
    }

    virtual void print(output_stream* os) const
    {
        std::string s = str();
        os->write_chars(s.c_str(), s.size());
    }

    virtual sockaddr* get_data() { return reinterpret_cast<sockaddr*>(&m_data); }
    virtual const sockaddr* get_data() const { return reinterpret_cast<const sockaddr*>(&m_data); }
    virtual int get_length() const { return sizeof(sockaddr_in); }

    const sockaddr_in& get_v4_data() const { return m_data; }
    sockaddr_in& get_v4_data() { return m_data; }

    uint32_t get_ip() const
    {
        return ntohl(m_data.sin_addr.s_addr);
    }
    void set_ip(uint32_t ip)
    {
        m_data.sin_addr.s_addr = htonl(ip);
    }

    uint16_t get_port() const
    {
        return ntohs(m_data.sin_port);
    }
    void set_port(uint16_t port)
    {
        m_data.sin_port = htons(port);
    }

    uint32_t get_raw_ip() const
    {
        return m_data.sin_addr.s_addr;
    }
    void set_raw_ip(uint32_t ip)
    {
        m_data.sin_addr.s_addr = ip;
    }

    uint16_t get_raw_port() const
    {
        return m_data.sin_port;
    }
    void set_raw_port(uint16_t port)
    {
        m_data.sin_port = port;
    }

    void set_ip(const char* ipstr)
    {
        if (NULL == ipstr)
            return;
        set_raw_ip(inet_addr(ipstr));
    }

    void clear()
    {
        XUL_FILL_ZERO(m_data);
        m_data.sin_family = AF_INET;
    }

    inet4_address get_inet_address() const
    {
        inet4_address ip;
        ip.set_raw_address(get_raw_ip());
        return ip;
    }

    std::string str() const;

    bool try_parse(const std::string& s)
    {
        std::string::size_type pos = s.find(':');
        if (pos == std::string::npos)
            return false;
        std::string ipStr = s.substr(0, pos);
        std::string portStr = s.substr(pos + 1);
        if (ipStr.empty() || portStr.empty())
            return false;
        this->set_ip(ipStr.c_str());
        this->set_port(xul::numerics::parse<uint16_t>(portStr, 0));
        return is_valid();
    }

    bool is_valid() const
    {
        return get_inet_address().is_valid() && get_raw_port() != 0;
    }
    bool assign(const sockaddr& addr)
    {
        if (addr.sa_family != AF_INET)
            return false;
        assert(sizeof(sockaddr_in) == sizeof(sockaddr));
        memcpy(&m_data, &addr, sizeof(addr));
        return true;
    }
    bool assign(const socket_address& addr)
    {
        if (this == &addr)
            return false;
        if (addr.get_length() != this->get_length() || addr.get_data()->sa_family != AF_INET)
            return false;
        memcpy(&m_data, addr.get_data(), sizeof(m_data));
        return true;
    }

private:
    sockaddr_in m_data;
};

inline bool operator==(const inet_socket_address& x, const inet_socket_address& y)
{
    return x.get_raw_ip() == y.get_raw_ip() && x.get_raw_port() == y.get_raw_port();
}

inline bool operator!=(const inet_socket_address& x, const inet_socket_address& y)
{
    return !(x == y);
}

inline bool operator<(const xul::inet_socket_address& x, const xul::inet_socket_address& y)
{
    if (x.get_raw_ip() != y.get_raw_ip())
        return (x.get_raw_ip() < y.get_raw_ip());
    return x.get_raw_port() < y.get_raw_port();
}

inline std::ostream& operator<<(std::ostream& os, const inet_socket_address& addr)
{
    inet4_address ip = addr.get_inet_address();
    return os << ip << ":" << addr.get_port();
}

inline std::string inet_socket_address::str() const
{
    std::ostringstream os;
    os << *this;
    return os.str();
}

}

