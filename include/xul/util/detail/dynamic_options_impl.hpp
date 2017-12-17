#pragma once

#include <xul/util/detail/options_impl.hpp>
#include <xul/util/options_wrapper.hpp>

/**
 * @file
 * @brief configuration base class
 */


namespace xul {


class dynamic_options_impl : public options_impl
{
public:
    virtual bool set(const char* name, const char* val)
    {
        if (!name)
            return false;
        option* opt = m_items->get_ref(name, NULL);
        if (opt)
        {
            return opt->parse(val);
        }
        std::string namestr(name);
        std::string::size_type pos = namestr.find('.');
        if (std::string::npos == pos)
        {
            options_wrapper(*this).add_string(namestr, "");
            opt = m_items->get_ref(name, NULL);
            if (opt)
            {
                return opt->parse(val);
            }
            assert(false);
            return false;
        }
        std::string nodename = namestr.substr(0, pos);
        options* opts = m_nodes->get_ref(nodename.c_str(), NULL);
        if (!opts)
        {
            opts = new dynamic_options_impl;
            add_options(nodename.c_str(), opts);
        }
        std::string node_item_name = namestr.substr(pos + 1);
        return opts->set(node_item_name.c_str(), val);
    }
};


options* create_dynamic_options()
{
    return new dynamic_options_impl;
}


}
