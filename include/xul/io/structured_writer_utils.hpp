#pragma once

#include <xul/io/structured_writer.hpp>
#include <xul/data/tuple.hpp>
#include <xul/std/strings.hpp>
#include <xul/macro/minmax.hpp>
#include <xul/util/time_counter.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <stdint.h>

namespace xul {


/// utility functions for structured_writer
class structured_writer_utils
{
public:
    static void write_time_tuple(structured_writer& writer, const char* name, const time_counter& tc)
    {
        boost::posix_time::time_duration duration = tc.get_elapsed_time();
        boost::posix_time::ptime systime = boost::posix_time::microsec_clock::universal_time() - duration;
        writer.write_string(name, xul::make_tuple(systime, duration));
    }

    template <typename ElemtntT>
    static void write_object(structured_writer& writer, const char* name, const ElemtntT& elem)
    {
        writer.start_dict(name);
        elem.write(writer);
        writer.end_dict();
    }

    template <typename ElemtntT>
    static void write_array_item(structured_writer& writer, const char* name, const ElemtntT& elem)
    {
        write_object(name, elem, true);
    }

    template <typename ElemtntT>
    static void write_array(structured_writer& writer, const char* name, const char* itemName, const std::vector<ElemtntT>& elems)
    {
        write_array(name, itemName, elems.empty() ? NULL : &elems[0], elems.size());
    }

    template <typename ElemtntT>
    static void write_array(structured_writer& writer, const char* name, const char* itemName, const std::vector<boost::shared_ptr<const ElemtntT> >& elems)
    {
        write_array(name, itemName, elems.empty() ? NULL : &elems[0], elems.size());
    }

    template <typename ElemtntT>
    static void write_array(structured_writer& writer, const char* name, const char* itemName, const ElemtntT* elems, size_t size)
    {
        //array_entry entry(*this, name, size);
        writer.start_list(name, size);
        for (int i = 0; i < size; ++i)
        {
            write_array_item(itemName, elems[i]);
        }
        writer.end_list();
    }

    template <typename ElemtntT>
    static void write_array(structured_writer& writer, const char* name, const char* itemName, const boost::shared_ptr<const ElemtntT>* elems, size_t size)
    {
        //array_entry entry(*this, name, size);
        writer.start_list(name, size);
        for (int i = 0; i < size; ++i)
        {
            write_array_item(itemName, *elems[i]);
        }
        writer.end_list();
    }

    static void write_string_array(structured_writer& writer, const char* name, const char* itemName, const std::vector<std::string>& items)
    {
        //array_entry entry(*this, name, items.size());
        writer.start_list(name, items.size());
        for (int i = 0; i < items.size(); ++i)
        {
            writer.write_string(itemName, items[i]);
        }
        writer.end_list();
    }
};


}
