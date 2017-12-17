
#include <xul/config.hpp>

#include <xul/data/associative_string_array.hpp>
#include <xul/data/string_table.hpp>
#include <xul/data/tree_map.hpp>
#include <xul/data/array_list.hpp>
#include <xul/data/detail/variant_impl.hpp>
#include <xul/data/detail/string_tokenizer_impl.hpp>

#include <xul/lang/detail/class_info_impl.hpp>

#include <xul/text/xml_writer.hpp>
#include <xul/text/json_writer.hpp>

#include <xul/lib/library.hpp>
#include <xul/lib/library_class.hpp>
#include <xul/lib/library_repository.hpp>
#include <xul/lib/detail/library_info_impl.hpp>
#include <xul/lang/detail/class_info_impl.hpp>
#include <xul/lang/detail/global_service_manager_impl.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


typedef tree_map<const char*, const char*> string_tree_map;
typedef tree_map<const char*, const char*, istring_key_element_storage_traits> istring_tree_map;

variant* create_variant()
{
    return new detail::variant_impl;
}

std::ostream& operator<<(std::ostream& os, const variant::list_type& vars)
{
    os << "[";
    for (int i = 0; i < vars.size(); ++i)
    {
        const variant* item = vars.get(i);
        os << *item << ", ";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const xul::variant& var)
{
    int vtype = var.get_type();
    switch (vtype)
    {
    case variant_type_null:
        os << "var:null";
        break;
    case variant_type_bool:
        {
            bool val = var.get_bool(false);
            os << "var:" << (val ? "true" : "false");
        }
        break;
    case variant_type_integer:
        {
            int64_t val = var.get_integer(0);
            os << "var:" << val;
        }
        break;
    case variant_type_float:
        {
            double val = var.get_float(0);
            os << "var:float:" << std::fixed << val;
        }
        break;
    case variant_type_string:
        {
            int len = 0;
            const char* str = var.get_bytes(&len);
            assert(NULL != str || 0 == len);
            os << "var:\"" << std::string(str ? str : "", len) << '"';
        }
        break;
    case variant_type_list:
        {
            const variant::list_type* items = var.get_list();
            os << *items;
        }
        break;
    case variant_type_dict:
        {
            const variant::dict_type* items = var.get_dict();
            os << "{";
            XUL_ITERATE(variant::dict_type, items, iter)
            {
                const variant::dict_type::entry_type* entry = iter->element();
                os << '"' << entry->get_key() << "\": " << *entry->get_value() << ", ";
            }
            os << "}";
        }
        break;
    case variant_type_taged_buffer:
        {
            const variant_tagged_buffer* tag = var.get_tagged_buffer();
            os << "var:tagged_buffer:" << (tag ? tag->get_tag() : "") << ":" << (tag ? tag->get_size() : 0);
        }
        break;
    default:
        os << "var:unknown:" << vtype;
        break;
    }
    return os;
}

list<variant*>* create_variant_list()
{
    return new array_list<variant*>;
}
map<const char*, variant*>* create_variant_dict()
{
    return new tree_map<const char*, variant*>;
}

string_table* create_string_map()
{
    return new string_tree_map;
}

string_table* create_istring_map()
{
    return new istring_tree_map;
}

string_table* create_associative_string_array()
{
    return new associative_string_array<>;
}

string_table* create_associative_istring_array()
{
    return new associative_istring_array;
}

string_tokenizer* create_string_tokenizer()
{
    return new detail::string_tokenizer_impl;
}


structured_writer* create_xml_writer(output_stream* os, bool compressed)
{
    return new xml_writer(os, compressed);
}

structured_writer* create_json_writer(output_stream* os, bool compressed)
{
    return new json_writer(os, compressed);
}
    

class_info* create_class_info(const char* name)
{
    return new class_info_impl(name);
}

}
