#pragma once

#include <xul/std/maps.hpp>
#include <xul/util/singleton.hpp>
#include <string>

namespace xul {


class well_known_ports
{
public:
    class port_table : public singleton<const port_table>
    {
    public:
        std::map<std::string, int> ports;
        port_table()
        {
            ports["ftp"] = 21;
            ports["ssh"] = 22;
            ports["telnet"] = 23;
            ports["smtp"] = 25;
            ports["dns"] = 53;
            ports["http"] = 80;
            ports["pop"] = 110;
            ports["nntp"] = 119;
            ports["imap"] = 143;
            ports["snmp"] = 161;
            ports["https"] = 443;
            ports["rtsp"] = 554;
            ports["rtmp"] = 1935;
            ports["rtmfp"] = 1935;
        }
        int get_port(const std::string& scheme, int defaultPort) const
        {
            return maps::get(ports, scheme, defaultPort);
        }
    };

    static const port_table& get_port_table()
    {
        return port_table::instance();
    }

    static int get_port(const std::string& scheme, int defaultPort)
    {
        return get_port_table().get_port(scheme, defaultPort);
    }
};

}
