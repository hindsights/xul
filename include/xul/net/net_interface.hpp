#pragma once

#include <xul/net/inet4_address.hpp>
#include <xul/text/hex_encoding.hpp>
#include <xul/std/strings.hpp>
#include <string>
#include <stdint.h>



namespace xul {


/// reperesent 6 bytes of hardware address
class mac_address
{
public:
    enum { address_size = 6 };

    mac_address()
    {
        clear();
    }

    void clear()
    {
        memset(m_bytes, 0, address_size);
    }

    void assign(const uint8_t* data)
    {
        assert(data != NULL);
        memcpy(m_bytes, data, address_size);
    }

    std::string str() const
    {
        return strings::format("%02X-%02X-%02X-%02X-%02X-%02X",
            m_bytes[0], m_bytes[1], m_bytes[2], m_bytes[3], m_bytes[4], m_bytes[5]);
    }
    std::string encode() const
    {
        return hex_encoding::upper_case().encode(m_bytes, address_size);
    }
    const uint8_t* bytes() const { return m_bytes; }
    uint8_t* buffer() { return m_bytes; }
    size_t size() const { return address_size; }

private:
    uint8_t m_bytes[address_size];
};


/// interface of network adpater card
class net_interface
{
public:
    const mac_address& get_hardware_address() const { return m_hardware_address; }
    mac_address& get_hardware_address() { return m_hardware_address; }
    void set_hardware_address(const mac_address& addr) { m_hardware_address = addr; }

    const inet4_address& get_ip_address() const { return m_ip_address; }
    inet4_address& get_ip_address() { return m_ip_address; }
    void set_ip_address(const inet4_address& addr) { m_ip_address = addr; }

    const inet4_address& get_gateway_address() const { return m_gateway_address; }
    inet4_address& get_gateway_address() { return m_gateway_address; }
    void set_gateway_address(const inet4_address& addr) { m_gateway_address = addr; }
    
    const std::string& get_name() const { return m_name; }
    void set_name(const std::string& name) { m_name = name; }

private:
    std::string m_name;
    mac_address m_hardware_address;
    inet4_address m_ip_address;
    inet4_address m_gateway_address;
};


}
