#pragma once

#include <xul/net/dns_backup.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/data/array_list.hpp>
#include <xul/net/inet4_address.hpp>
#include <xul/log/log.hpp>
#include <assert.h>


namespace xul {


class dns_backup_impl : public object_impl<dns_backup>
{
public:
    typedef list<uint32_t> ipv4_address_list;
    typedef boost::intrusive_ptr<ipv4_address_list> ipv4_address_list_ptr;

    class dns_entry
    {
    public:
        std::string hostname;
        ipv4_address_list_ptr  ipv4_addresses;
    };

    dns_backup_impl()
    {
        XUL_LOGGER_INIT("dns_backup");
        XUL_REL_INFO("new");
    }
    ~dns_backup_impl()
    {
        XUL_REL_INFO("delete " << m_entries.size());
    }

    virtual void clear()
    {
        XUL_REL_INFO("clear " << m_entries.size());
        m_entries.clear();
    }
    virtual bool remove(const char* hostname)
    {
        XUL_REL_INFO("remove " << m_entries.size() << " " << (hostname ? hostname : "(null)"));
        std::string namestr = (hostname ? hostname : "");
        if (namestr.empty())
        {
            assert(false);
            return false;
        }
        m_entries.erase(namestr);
        return true;
    }
    virtual bool add(const char* hostname, const char* ip)
    {
        std::string ipstr = (ip ? ip : "");
        inet4_address addr;
        addr.set_address(ipstr.c_str());
        if (!addr.is_valid())
        {
            XUL_REL_ERROR("add failed " << (hostname ? hostname : "(null)") << " " << ipstr);
            return false;
        }
        return add_ipv4(hostname, addr.get_raw_address());
    }
    virtual bool add_ipv4(const char* hostname, uint32_t ip)
    {
        XUL_REL_INFO("add_ipv4 " << m_entries.size() << " " << (hostname ? hostname : "(null)") << " " << ip << " " << xul::make_raw_inet4_address(ip));
        std::string namestr = (hostname ? hostname : "");
        if (namestr.empty())
        {
            assert(false);
            return false;
        }
        dns_entry& entry = m_entries[namestr];
        if (entry.hostname.empty() || !entry.ipv4_addresses)
        {
            entry.hostname = namestr;
            entry.ipv4_addresses = new xul::array_list<uint32_t>();
        }
        entry.ipv4_addresses->add(ip);
        return true;
    }
    virtual list<uint32_t>* lookup(const char* hostname) const
    {
        std::string namestr = (hostname ? hostname : "");
        if (namestr.empty())
        {
            assert(false);
            return NULL;
        }
        std::map<std::string, dns_entry>::const_iterator iter = m_entries.find(namestr);
        if (m_entries.end() == iter)
            return NULL;
        return iter->second.ipv4_addresses.get();
    }

private:
    XUL_LOGGER_DEFINE();
    std::map<std::string, dns_entry> m_entries;
};


dns_backup* dns_backup::get_global_backup()
{
    static boost::intrusive_ptr<dns_backup_impl> the_backup = new dns_backup_impl;
    return the_backup.get();
}


}
