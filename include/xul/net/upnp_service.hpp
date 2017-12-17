#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class upnp_service;


class upnp_service_listener
{
public:
    virtual void on_upnp_port_mapped(upnp_service* sender, int inner, int external, bool use_exiting) = 0;
    virtual void on_upnp_port_mapping_failed(upnp_service* sender, int inner, int external, int errcode) = 0;
};


class upnp_service : public object
{
public:
    virtual void stop() = 0;
    virtual void set_description(const char* desc) = 0;
    virtual void map_port(int internal_port, int external_port, const char* protocol, int lease_duration) = 0;
    virtual void set_listener(upnp_service_listener* listener) = 0;
};


}
