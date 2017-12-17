#pragma once

#include <xul/util/options.hpp>
#include <xul/util/option.hpp>
#include <xul/data/tree_map.hpp>
#include <xul/util/data_parser.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/macro/iterate.hpp>

/**
 * @file
 * @brief configuration base class
 */


namespace xul {


class options_impl : public object_impl<options>
{
public:
    typedef map<const char*, option*> option_map;
    typedef map<const char*, options*> options_map;

    options_impl()
    {
        m_nodes = new tree_map<const char*, options*>;
        m_items = new tree_map<const char*, option*>;
    }

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
            return false;
        std::string nodename = namestr.substr(0, pos);
        options* opts = m_nodes->get_ref(nodename.c_str(), NULL);
        if (!opts)
            return false;
        std::string node_item_name = namestr.substr(pos + 1);
        return opts->set(node_item_name.c_str(), val);
    }
    //virtual const char* get(const char* name, const char* default_val) const = 0;
    virtual void reset()
    {
        XUL_ITERATE_REF(options_map, m_nodes, iter)
        {
            iter->element()->ref_value()->reset();
        }
        XUL_ITERATE_REF(option_map, m_items, iter)
        {
            iter->element()->ref_value()->reset();
        }
    }
    virtual void add(option* opt)
    {
        if (!opt)
            return;
        m_items->set(opt->get_name(), opt);
    }
    virtual void add_options(const char* name, options* opts)
    {
        m_nodes->set(name, opts);
    }

    virtual option* get_option(const char* name)
    {
        if (!name)
            return NULL;
        return m_items->get_ref(name, NULL);
    }

    virtual options* get_options(const char* name)
    {
        if (!name)
            return NULL;
        return m_nodes->get_ref(name, NULL);
    }

    virtual const options_node_map* get_nodes() const
    {
        return m_nodes.get();
    }
    virtual options_node_map* ref_nodes()
    {
        return m_nodes.get();
    }
    virtual const option_item_map* get_items() const
    {
        return m_items.get();
    }
    virtual option_item_map* ref_items()
    {
        return m_items.get();
    }

    virtual void mark_initialized()
    {
        XUL_ITERATE_REF(options_map, m_nodes, iter)
        {
            iter->element()->ref_value()->mark_initialized();
        }
        XUL_ITERATE_REF(option_map, m_items, iter)
        {
            iter->element()->ref_value()->mark_initialized();
        }
    }
    virtual void set_public(const char* name, bool is_public)
    {
        //options_node_map::iterator node_iter = m_nodes->get_items().find(name);
        //if (node_iter != m_nodes->get_items().end())
        //    node_iter
        option* opt = get_option(name);
        if (opt)
        {
            opt->set_public(is_public);
        }
    }

    virtual void freeze(const char* name)
    {
        //options_node_map::iterator node_iter = m_nodes->get_items().find(name);
        //if (node_iter != m_nodes->get_items().end())
        //    node_iter
        option* opt = get_option(name);
        if (opt)
        {
            opt->freeze();
        }
    }
    virtual void dump(structured_writer* writer, int level) const
    {
        XUL_ITERATE(options_map, m_nodes, iter)
        {
            writer->start_dict(iter->element()->get_key());
            iter->element()->get_value()->dump(writer, level);
            writer->end_dict();
        }
        XUL_ITERATE(option_map, m_items, iter)
        {
            iter->element()->get_value()->dump(writer, iter->element()->get_key());
        }
    }

protected:
    boost::intrusive_ptr<option_map> m_items;
    boost::intrusive_ptr<options_map> m_nodes;
};


options* create_options()
{
    return new options_impl;
}


}
