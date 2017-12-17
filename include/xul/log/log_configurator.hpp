#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class log_filter_chain;
class options;


class log_configurator : public object
{
public:
    virtual bool configure(log_service* service, const char* filepath) = 0;
    virtual log_filter_chain* configure_filter_chain(options* opts) = 0;
    //virtual log_filter_chain* configure_filter_chain(const log_config*) = 0;
};


log_configurator* create_properties_log_configurator();


}
