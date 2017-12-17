#pragma once

#include <xul/net/net_interface.hpp>
#include <xul/log/log.hpp>
#include <xul/text/hex_encoding.hpp>
#include <xul/data/buffer.hpp>
#include <boost/config.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <xul/config.hpp>
#include <vector>
#include <map>
#include <stdint.h>

#if defined(XUL_WINDOWS)
#include <xul/net/inet4_address.hpp>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#if defined(XUL_MACH)
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#else
#include <net/if_arp.h>
#endif
#include <xul/net/sockets.hpp>
#include <xul/net/detail/bsd_socket.hpp>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#endif



namespace xul {


/// manage network interfaces
class net_interface_manager
{
public:
#if defined(XUL_WINDOWS)
    static void list(std::vector<net_interface>& interfaces)
    {
        interfaces.clear();
        IP_ADAPTER_INFO* table = NULL;
        ULONG tablesize = 0;
        DWORD ret = ::GetAdaptersInfo(table, &tablesize);
        if (ret != NO_ERROR && 0 == tablesize)
        {
            XUL_APP_ERROR("GetAdaptersInfo failed " << xul::make_tuple(ret, tablesize));
            return;
        }
        xul::byte_buffer buf;
        buf.resize(tablesize);
        table = reinterpret_cast<IP_ADAPTER_INFO*>(buf.data());
        ret = ::GetAdaptersInfo(table, &tablesize);
        if (ret != NO_ERROR)
        {
            XUL_APP_ERROR("GetAdaptersInfo failed " << xul::make_tuple(ret, tablesize));
            return;
        }
        XUL_APP_DEBUG("GetAdaptersInfo get entries: " << tablesize);
        for (int i = 0; table; table = table->Next, ++i)
        {
            //const MIB_IPNETROW& row = table->table[i];
            if (table->AddressLength != 6)
                continue;
            net_interface netif;
            netif.get_ip_address().set_address(table->IpAddressList.IpAddress.String);
            netif.get_gateway_address().set_address(table->GatewayList.IpAddress.String);
            if (false == netif.get_ip_address().is_valid())
                continue;

            memcpy(netif.get_hardware_address().buffer(), table->Address, 6);
            interfaces.push_back(netif);

            XUL_APP_DEBUG("GetAdaptersInfo entry " << xul::make_tuple(i, netif.get_ip_address(), table->Type, netif.get_hardware_address().str()));
        }
    }
#elif defined(XUL_MACH)
#if 1
    static bool list(std::vector<net_interface>& interfaces)
    {
        interfaces.clear();
        int  success;
        struct ifaddrs * addrs;
        struct ifaddrs * cursor;
        const struct sockaddr_dl * dlAddr;
        const struct sockaddr_in * inAddr;
        const unsigned char* base;
        int i;
        success = getifaddrs(&addrs) == 0;
        std::map<std::string, net_interface> ifs;
        if (success)
        {
            cursor = addrs;
            while (cursor != 0)
            {
                if ((cursor->ifa_flags & IFF_LOOPBACK) == 0)
                {
                    net_interface& netif = ifs[cursor->ifa_name];
                    if (netif.get_name().empty())
                        netif.set_name(cursor->ifa_name);
                    if (cursor->ifa_addr->sa_family == AF_LINK)
                    {
                        dlAddr = (const struct sockaddr_dl *) cursor->ifa_addr;
                        base = (const unsigned char*) &dlAddr->sdl_data[dlAddr->sdl_nlen];
                        //netif.set_ip_address((const sockaddr_in));
                        netif.get_hardware_address().assign(base);
                    }
                    else if (cursor->ifa_addr->sa_family == AF_INET)
                    {
                        inAddr = (const struct sockaddr_in*)cursor->ifa_addr;
                        netif.get_ip_address().set_raw_address(inAddr->sin_addr.s_addr);
                    }
                }
                cursor = cursor->ifa_next;
            }

            freeifaddrs(addrs);
        }
        for (std::map<std::string, net_interface>::const_iterator iter = ifs.begin(); iter != ifs.end(); ++iter)
        {
            const net_interface& netif = iter->second;
            if (netif.get_ip_address().is_valid())
            {
                XUL_TRACE_LOG("add network interface " << netif.get_name() << " " << netif.get_ip_address().str());
                interfaces.push_back(netif);
            }
        }
        XUL_APP_DEBUG("list interfaces: " << interfaces.size());
        return true;
    }
#else
#pragma mark MAC
    // Return the local MAC addy
    // Courtesy of FreeBSD hackers email list
    // Accidentally munged during previous update. Fixed thanks to mlamb.
    static bool list(std::vector<net_interface>& interfaces)
    {
        int mib[6];
        size_t len;
        char *buf;
        unsigned char *ptr;
        struct if_msghdr *ifm;
        struct sockaddr_dl *sdl;

        mib[0] = CTL_NET;
        mib[1] = AF_ROUTE;
        mib[2] = 0;
        mib[3] = 0;
        mib[4] = NET_RT_IFLIST;

        if ((mib[5] = if_nametoindex("en0")) == 0)
        {
            XUL_APP_DEBUG("Error: if_nametoindex error");
            return false;
        }

        if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
        {
            XUL_APP_DEBUG("Error: sysctl, take 1");
            return false;
        }

        if ((buf = (char*)malloc(len)) == NULL)
        {
            XUL_APP_DEBUG("Could not allocate memory. error!");
            return false;
        }

        if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
        {
            XUL_APP_DEBUG("Error: sysctl, take 2");
            return false;
        }

        ifm = (struct if_msghdr *)buf;
        sdl = (struct sockaddr_dl *)(ifm + 1);
        ptr = (unsigned char *)LLADDR(sdl);
        // NSString *outstring = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
        //NSString *outstring = [NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];

        net_interface netif;
        memcpy(netif.get_hardware_address().buffer(), ptr, 6);
        interfaces.push_back(netif);

        free(buf);

        return true;
        //return [outstring uppercaseString];
    }
#endif
#else
    static bool list(std::vector<net_interface>& interfaces)
    {
        const int MAXINTERFACES = 16;
        struct ifreq ifreqs[MAXINTERFACES];

        interfaces.clear();
        xul::bsd_socket sock;
        if (false == sock.create(AF_INET, SOCK_DGRAM, 0))
        {
            XUL_APP_REL_ERROR("failed to list interface 0");
            return false;
        }
        struct ifconf ifc = { 0 };
        ifc.ifc_ifcu.ifcu_buf = (char*)ifreqs;
        ifc.ifc_len = sizeof(ifreqs);

        if (false == sock.ioctl(SIOCGIFCONF, (char*)&ifc))
        {
            XUL_APP_REL_ERROR("failed to list interface 2");
            return false;
        }
        int if_count = ifc.ifc_len / sizeof(ifreq);
        XUL_APP_REL_INFO("getifconf ok " << xul::make_tuple(ifc.ifc_len, sizeof(ifreq), if_count));
        struct ifreq* ifr = ifc.ifc_ifcu.ifcu_req;
        for (int i = 0; i < if_count; ++i)
        {
            struct ifreq* ifr = &ifreqs[i];
            std::string ifname = ifr->ifr_name;
            XUL_APP_REL_INFO("if: " << ifname);
            if (std::string("lo") == ifr->ifr_name)
                continue;
            net_interface netif;
            netif.get_ip_address().set_raw_address(((const sockaddr_in&)ifr->ifr_ifru.ifru_addr).sin_addr.s_addr);
            if (!sock.ioctl(SIOCGIFHWADDR, (char*)ifr))
            {
                XUL_APP_REL_WARN("failed to get hwaddr " << ifname << " " << netif.get_ip_address());
                continue;
            }
            netif.get_hardware_address().assign((const uint8_t*)ifr->ifr_ifru.ifru_hwaddr.sa_data);
            if (false == netif.get_ip_address().is_valid())
                continue;
            interfaces.push_back(netif);
            XUL_APP_REL_INFO("list interface: " << ifname << " " << netif.get_ip_address() << " " << netif.get_hardware_address().str());
        }
        XUL_APP_DEBUG("list interfaces: " << interfaces.size());
        return true;
    }
    bool get_mac_address(mac_address& addr)
    {
        xul::bsd_socket sock;
        if (false == sock.create(AF_INET, SOCK_DGRAM, 0))
            return false;
        struct ifreq req = { 0 };
        if (false == sock.ioctl(SIOCGIFHWADDR, (char*)&req))
            return false;
        memcpy(addr.buffer(), req.ifr_hwaddr.sa_data, 6);
        return true;
    }
#endif

};


}
