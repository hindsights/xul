#pragma once

#include <xul/util/options.hpp>
#include <xul/util/data_parser.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/macro/iterate.hpp>

/**
 * @file
 * @brief configuration base class
 */


namespace xul {


template <typename BaseT>
class options_proxy : public object_impl<BaseT>
{
public:
    options_proxy()
    {
        m_opts = create_options();
    }

    virtual const options::options_node_map* get_nodes() const { return m_opts->get_nodes(); }
    virtual options::options_node_map* ref_nodes() { return m_opts->ref_nodes(); }
    virtual const options::option_item_map* get_items() const { return m_opts->get_items(); }
    virtual options::option_item_map* ref_items() { return m_opts->ref_items(); }

    /// set option value
    virtual bool set(const char* name, const char* val) { return m_opts->set(name, val); }
    /// reset all options' values
    virtual void reset() { m_opts->reset(); }

    virtual void add(option* opt) { m_opts->add(opt); }
    virtual void add_options(const char* name, options* opts) { m_opts->add_options(name, opts); }
    virtual option* get_option(const char* name)
    {
        return m_opts->get_option(name);
    }
    virtual options* get_options(const char* name)
    {
        return m_opts->get_options(name);
    }
    virtual void mark_initialized()
    {
        m_opts->mark_initialized();
    }
    virtual void set_public(const char* name, bool is_public)
    {
        m_opts->set_public(name, is_public);
    }
    virtual void freeze(const char* name)
    {
        m_opts->freeze(name);
    }
    virtual void dump(structured_writer* writer, int level) const
    {
        m_opts->dump(writer, level);
    }
protected:
    xul::options& get_options()
    {
        return *m_opts;
    }
    const xul::options& get_options() const
    {
        return *m_opts;
    }

private:
    boost::intrusive_ptr<options> m_opts;
};


}
