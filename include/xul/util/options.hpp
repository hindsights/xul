#pragma once

#include <xul/util/option.hpp>
#include <xul/data/map.hpp>
#include <xul/lang/object.hpp>

/**
 * @file
 * @brief options interface
 */


namespace xul {


class option;
class structured_writer;


class options : public xul::object
{
public:
    typedef map<const char*, options*> options_node_map;
    typedef map<const char*, option*> option_item_map;
    //typedef dictionary_entry<const char*, const options*, object_element_traits<options> > const_options_node_entry;
    //typedef dictionary_entry<const char*, options*, object_element_traits<options> > options_node_entry;
    //typedef dictionary_entry<const char*, const option*, object_element_traits<option> > const_option_item_entry;
    //typedef dictionary_entry<const char*, option*, object_element_traits<option> > option_item_entry;

    virtual const options_node_map* get_nodes() const = 0;
    virtual options_node_map* ref_nodes() = 0;
    virtual const option_item_map* get_items() const = 0;
    virtual option_item_map* ref_items() = 0;
    //virtual void add_option(const char* name, option* opt) = 0;
    //virtual void remove_options() = 0;

    /// set option value
    virtual bool set(const char* name, const char* val) = 0;
    /// reset all options' values
    virtual void reset() = 0;

    virtual option* get_option(const char* name) = 0;
    virtual options* get_options(const char* name) = 0;
    virtual void add(option* opt) = 0;
    virtual void add_options(const char* name, options* opts) = 0;
    virtual void mark_initialized() = 0;
    virtual void set_public(const char* name, bool is_public) = 0;
    virtual void freeze(const char* name) = 0;
    virtual void dump(structured_writer* writer, int level) const = 0;
};


class root_options : public options
{
public:
    virtual bool parse_file(const char* filepath) = 0;
};


options* create_options();
options* create_dynamic_options();


}
