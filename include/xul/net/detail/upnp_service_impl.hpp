#pragma once

#include <xul/net/upnp_service.hpp>
#include <xul/net/udp_socket.hpp>
#include <xul/net/udp_socket_listener.hpp>
#include <xul/net/http/http_client.hpp>
#include <xul/net/http/http_response_decoder.hpp>
#include <xul/text/tinyxml.hpp>
#include <xul/os/paths.hpp>
#include <xul/log/log.hpp>
#include <xul/net/detail/upnp.hpp>
#include <boost/algorithm/string/predicate.hpp>


namespace xul {


class upnp_service_impl : public xul::object_impl<upnp_service>, public udp_socket_listener
{
public:
    class port_mapping_info
    {
    public:
        int internal_port;
        int external_port;
        int lease_duration;
        int enabled;
        std::string protocol;
        std::string description;
        std::string internal_client;

        port_mapping_info()
        {
            internal_port = 0;
            external_port = 0;
            lease_duration = 0;
            enabled = 1;
        }
    };

    explicit upnp_service_impl(io_service* ios, const std::vector<net_interface>& interfaces)
        : m_net_interfaces(interfaces)
        , m_last_search_time(0)
        , m_mapping_port_times(0)
    {
        XUL_LOGGER_INIT("upnp_service");
        XUL_DEBUG("new");
        m_client = new http_client(ios);
        m_udp = ios->create_udp_socket();
        m_udp->open(0);
        m_udp->set_listener(this);
        m_timer.create_periodic_timer(ios);
        m_timer.set_callback(boost::bind(&upnp_service_impl::on_tick, this));
        m_igd_uri = create_uri();
        m_control_uri = create_uri();
        m_description = "upnp";
        m_search_interval = 5000;
        m_listener = NULL;
        m_finished = true;
    }
    virtual ~upnp_service_impl()
    {
        XUL_DEBUG("delete");
    }
    virtual void stop()
    {
        m_udp->set_listener(NULL);
        m_udp->close();
    }

    virtual void set_description(const char* desc)
    {
        m_description = desc;
    }
    virtual void map_port(int internal_port, int external_port, const char* protocol, int lease_duration)
    {
        m_add_port_mapping_params.internal_port = internal_port;
        m_add_port_mapping_params.external_port = external_port;
        m_add_port_mapping_params.protocol = protocol;
        m_add_port_mapping_params.lease_duration = lease_duration;
        m_add_port_mapping_params.enabled = 1;
        m_add_port_mapping_params.description = m_description;
        m_finished = false;
        m_timer.start(1000);
        m_udp->receive(4096);
        XUL_REL_INFO("map_port " << make_tuple(internal_port, external_port, protocol, lease_duration));
        //boost::intrusive_ptr<uri> url = create_uri();
        //url->parse("http://239.255.255.250:1900*");
        search_root_device();
    }

    virtual void set_listener(upnp_service_listener* listener)
    {
        m_listener = listener;
    }

    void on_tick()
    {
        search_root_device();
    }

    virtual void on_socket_receive(udp_socket* sender, unsigned char* data, int size, inet_socket_address* remoteAddr)
    {
        XUL_DEBUG("on_socket_receive " << m_finished << " " << remoteAddr << " " << std::string(reinterpret_cast<const char*>(data), size));
        if (m_finished)
            return;
        m_udp->receive(4096);
        xul::http_response_extractor respDec;
        respDec.feed(data, size);
        assert(respDec.is_header_finished());
        boost::intrusive_ptr<url_response> resp = respDec.get_response();
        std::string location = resp->get_header("LOCATION", "");
        m_igd_url = location;
        m_igd_uri->parse(location.c_str());
        assert(!location.empty());
        if (!is_lan_ip(m_igd_uri->get_host()))
        {
            XUL_WARN("not gateway address, ignore." << m_igd_url);
            return;
        }
        m_client->async_download_data(location, 8*1024, boost::bind(&upnp_service_impl::on_igd, this, _1, _2, _3, _4, _5, _6));
    }
    virtual void on_socket_receive_failed(udp_socket* sender, int errcode)
    {
        XUL_ERROR("on_socket_receive_failed " << errcode);
        if (m_igd_url.empty())
            return;
        m_udp->receive(4096);
    }

private:
    void search_root_device()
    {
        if (!m_igd_url.empty())
        {
            XUL_DEBUG("igd path is found, stop timer");
            m_timer.stop();
            return;
        }
        if (m_last_search_time.elapsed() < m_search_interval)
            return;
        m_search_interval += 10000;
        if (m_search_interval > 300 * 10000)
        {
            XUL_DEBUG("tried too many times, stop timer");
            m_timer.stop();
        }
        std::string msg = UPNP_SEARCH_REQUEST_STRING;
        xul::inet_socket_address socket_address = xul::inet_socket_address("239.255.255.250", 1900);
        m_udp->send(msg.data(), msg.size(), &socket_address);
        m_last_search_time.sync();
        XUL_DEBUG("send search request");
    }
    void on_igd(http_client* client, boost::intrusive_ptr<uri> url, int errcode, const url_response* resp, const uint8_t* data, size_t size)
    {
        m_local_ip = m_client->get_connection()->get_local_address().get_inet_address().str();
        m_add_port_mapping_params.internal_client = m_local_ip;
        std::string xmlstr(reinterpret_cast<const char*>(data), size);
        XUL_DEBUG("on_igd " << xmlstr);
        parse_device_description(xmlstr);
    }
    bool parse_device_description(const std::string& xmlstr)
    {
        tinyxml_document doc;
        if (!doc.parse(xmlstr.c_str()))
        {
            XUL_ERROR("failed parse igd xml");
            return false;
        }
        tinyxml_element rootElem = doc.get_root();
        if (!rootElem)
            return false;
        m_base_url = rootElem.get_string("URLBase");
        //rootElem = rootElem.get_first_child("root");
        //if (!rootElem)
        //    return false;
        const char* device_types[3] = { UPNP_DEVICE_TYPE_1, UPNP_DEVICE_TYPE_2, UPNP_DEVICE_TYPE_3 };
        tinyxml_element devElem = parse_device(rootElem, device_types, 0);
        if (!devElem)
            return false;
        tinyxml_element serviceListElem = devElem.get_first_child("serviceList");
        if (!serviceListElem)
            return false;
        tinyxml_element serviceElem = serviceListElem.get_first_child("service");
        if (!serviceElem)
            return false;
        std::string serviceType = serviceElem.get_string("serviceType");
        if (UPNP_SERVICE_WANIP != serviceType && UPNP_SERVICE_WANPPP != serviceType)
            return false;
        m_service_type = serviceType;
        std::string control_url = serviceElem.get_string("controlURL");
        if (control_url.empty())
            return false;
        if (boost::algorithm::istarts_with(control_url, "http://"))
        {
            m_control_url = control_url;
        }
        else
        {
            m_control_url = m_base_url + control_url;
            if (!boost::algorithm::istarts_with(control_url, "http://"))
            {
                boost::intrusive_ptr<uri> url = create_uri();
                url->parse(m_igd_url.c_str());
                if (strlen(url->get_host()) <= 0)
                    return false;
                if (m_control_url[0] == '/')
                    m_control_url = strings::format("http://%s:%d", url->get_host(), url->get_port()) + m_control_url;
                else
                    m_control_url = strings::format("http://%s:%d%s/", url->get_host(), url->get_port(), url_paths::get_directory(url->get_path()).c_str()) + m_control_url;
            }
        }
        m_control_uri->parse(m_control_url.c_str());
        XUL_DEBUG("found control_url " << m_control_url);
        //add_port_mapping(m_add_port_mapping_params);
        get_mapped_port(0);
        return true;
    }
    template <typename CallbackT>
    void upnp_request(const std::string& params, const char* action, CallbackT callback)
    {
        std::string body = strings::format(UPNP_SOAP_ACTION, action, m_service_type.c_str(), params.c_str(), action);
        boost::intrusive_ptr<string_table> headers = create_associative_istring_array();
        headers->set("Content-Type", "text/xml ; charset=\"utf-8\"");
        headers->set("SOAPACTION", strings::format("\"%s#%s\"", m_service_type.c_str(), action).c_str());
        boost::intrusive_ptr<uri> controlUri = create_uri();
        controlUri->parse(m_control_url.c_str());
        XUL_DEBUG("upnp_request soap request " << body);
        m_client->async_post_data(m_control_uri.get(), 4096, callback, body, "text/xml ; charset=\"utf-8\"", headers.get());
    }
    void get_mapped_port(int index)
    {
        XUL_DEBUG("get_mapped_port " << index);
        std::string params = strings::format(UPNP_GET_GEN_PARAMS, index);
        upnp_request(params, UPNP_ACTION_GET_GEN, boost::bind(&upnp_service_impl::on_get_mapped_port, this, index, _1, _2, _3, _4, _5, _6));
    }
    void on_get_mapped_port(int mapping_index, http_client* client, boost::intrusive_ptr<uri> url, int errcode, const url_response* resp, const uint8_t* data, size_t size)
    {
        if (resp && resp->get_status_code() >= 200 && resp->get_status_code() < 300)
        {
            port_mapping_info info;
            if (parse_port_mapping_info(&info, data, size))
            {
                if (info.internal_port == m_add_port_mapping_params.internal_port
                    && strings::iequals(info.protocol, m_add_port_mapping_params.protocol)
                    && info.internal_client == m_local_ip
                    && info.enabled)
                {
                    XUL_REL_INFO("found existing port mapping " << xul::make_tuple(info.internal_port, info.external_port, info.protocol, info.description));
                    m_timer.stop();
                    m_finished = true;
                    if (m_listener)
                        m_listener->on_upnp_port_mapped(this, m_add_port_mapping_params.internal_port, info.external_port, true);
                    return;
                }
                else
                {
                    if (mapping_index > 100)
                    {
                        m_timer.stop();
                        m_finished = true;
                        if (m_listener)
                            m_listener->on_upnp_port_mapping_failed(this, m_add_port_mapping_params.internal_port, m_add_port_mapping_params.external_port, -1);
                    }
                    else
                        get_mapped_port(mapping_index + 1);
                }
            }
            else
            {
                XUL_ERROR("failed to parse_port_mapping_info " << size);
                m_timer.stop();
                m_finished = true;
                if (m_listener)
                    m_listener->on_upnp_port_mapping_failed(this, m_add_port_mapping_params.internal_port, m_add_port_mapping_params.external_port, -2);
            }
        }
        else
        {
            XUL_ERROR("on_get_mapped_port failed " << make_tuple(errcode, resp ? resp->get_status_code() : 0));
            add_port_mapping(m_add_port_mapping_params);
        }
    }
    void add_port_mapping(const port_mapping_info& params)
    {
        XUL_DEBUG("add_port_mapping " << make_tuple(params.internal_port, params.external_port, params.protocol, params.lease_duration) << " " << m_description);
        std::string paramsstr = strings::format(
            UPNP_ADD_PORT_MAPPING_PARAMS,
            params.external_port,
            params.protocol.c_str(),
            params.internal_port,
            params.internal_client.c_str(),
            params.description.c_str(),
            params.lease_duration);
        upnp_request(paramsstr, UPNP_ACTION_ADD, boost::bind(&upnp_service_impl::on_add_mapping, this, _1, _2, _3, _4, _5, _6));
    }
    void on_add_mapping(http_client* client, boost::intrusive_ptr<uri> url, int errcode, const url_response* resp, const uint8_t* data, size_t size)
    {
        std::string xmlstr(reinterpret_cast<const char*>(data), size);
        XUL_DEBUG("on_add_mapping " << xmlstr);
        if (resp && resp->get_status_code() >= 200 && resp->get_status_code() < 300)
        {
            if (m_listener)
                m_listener->on_upnp_port_mapped(this, m_add_port_mapping_params.internal_port, m_add_port_mapping_params.external_port, false);
            return;
        }

        if (m_mapping_port_times++ < 5)
        {
            XUL_ERROR("on_get_mapped_port failed to retry" << make_tuple(errcode, resp ? resp->get_status_code() : 0));
            m_add_port_mapping_params.external_port += m_mapping_port_times;
            add_port_mapping(m_add_port_mapping_params);
        }
        else
        {
            if (m_listener)
                m_listener->on_upnp_port_mapping_failed(this, m_add_port_mapping_params.internal_port, m_add_port_mapping_params.external_port, -3);
        }
    }
    tinyxml_element parse_device(tinyxml_element parentElem, const char** deviceTypes, int depth)
    {
        assert(depth < 3);
        for (
            tinyxml_element deviceElem = parentElem.get_first_child("device");
            deviceElem;
            deviceElem = deviceElem.get_next_sibling())
        {
            std::string deviceType = deviceElem.get_string("deviceType");
            XUL_DEBUG("parse_device deviceType " << deviceType);
            if (deviceTypes[depth] == deviceType)
            {
                if (depth >= 2)
                {
                    return deviceElem;
                }
                tinyxml_element deviceListElem = deviceElem.get_first_child("deviceList");
                return parse_device(deviceListElem, deviceTypes, depth + 1);
            }
        }
        return tinyxml_element();
    }
    bool parse_port_mapping_info(port_mapping_info* info, const uint8_t* data, int size)
    {
        assert(info);
        std::string xmlstr(reinterpret_cast<const char*>(data), size);
        XUL_DEBUG("on_get_mapped_port " << xmlstr);
        tinyxml_document doc;
        if (!doc.parse(xmlstr.c_str()))
            return false;
        tinyxml_element elem = doc.get_root();
        if (!elem)
            return false;
        std::vector<tinyxml_element> elems;
        elem.get_elements_by_tag_name("u:GetGenericPortMappingEntryResponse", &elems);
        if (elems.empty())
            return false;
        elem = elems[0];
        info->description = elem.get_string("NewPortMappingDescription");
        info->external_port = elem.get_int("NewExternalPort", 0);
        info->internal_port = elem.get_int("NewInternalPort", 0);
        info->protocol = elem.get_string("NewProtocol");
        info->internal_client = elem.get_string("NewInternalClient");
        info->enabled = elem.get_int("NewEnabled", 0);
        info->lease_duration = elem.get_int("NewLeaseDuration", 0);
        return true;
    }

    bool is_lan_ip(const char* host)
    {
        inet4_address addr;
        addr.set_address(host);
        uint32_t ip = addr.get_address();
        return is_lan_ip(ip);
    }
    bool is_lan_ip(const unsigned int ip_addr)
    {
        if (ip_addr >= 0x0A000000 && ip_addr <= 0x0AFFFFFF)
            return true;
        if (ip_addr >= 0xAC100000 && ip_addr <= 0xAC1FFFFF)
            return true;
        if (ip_addr >= 0xC0A80000 && ip_addr <= 0xC0A8FFFF)
            return true;
        return false;
    }
    bool is_gateway_address(const std::string& host) const
    {
        for (int i = 0; i < m_net_interfaces.size(); ++i)
        {
            if (m_net_interfaces[i].get_gateway_address().str() == host)
                return true;
        }
        return false;
    }

private:
    XUL_LOGGER_DEFINE();
    std::vector<net_interface> m_net_interfaces;
    std::string m_local_ip;
    std::string m_igd_url;
    boost::intrusive_ptr<uri> m_igd_uri;
    std::string m_base_url;
    std::string m_control_url;
    boost::intrusive_ptr<uri> m_control_uri;
    boost::intrusive_ptr<udp_socket> m_udp;
    boost::intrusive_ptr<http_client> m_client;
    timer_holder m_timer;
    port_mapping_info m_add_port_mapping_params;
    std::string m_service_type;
    std::string m_description;
    int m_search_interval;
    time_counter m_last_search_time;
    upnp_service_listener* m_listener;
    int m_mapping_port_times;
    bool m_finished;
};


}
