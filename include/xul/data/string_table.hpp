#pragma once

#include <xul/lang/object.hpp>
#include <xul/data/map.hpp>


namespace xul {


/// interface for string table
typedef map<const char*, const char*> string_table;


string_table* create_string_map();
string_table* create_istring_map();
string_table* create_associative_string_array();
string_table* create_associative_istring_array();


}
