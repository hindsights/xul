#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/variant.hpp>


namespace xul {


/// interface for xml/json/yml/bencode/amf reader
class structured_reader : public object
{
public:
    virtual variant* read() = 0;
    virtual bool try_read(variant* var) = 0;
    virtual variant::list_type* read_list() = 0;
    virtual bool try_read_list(variant::list_type* vars) = 0;
};


}
