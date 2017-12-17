#pragma once

#include <xul/lib/global_service_manager.hpp>


#define XUL_LIBRARY_DEFINE_CLASS(info_class_name, class_name_str, class_type)        \
class info_class_name : public xul::class_info_impl                                    \
{                                                                                    \
public:                                                                                \
    info_class_name() : xul::class_info_impl(class_name_str) { }                    \
    virtual xul::object* create_object() { return new class_type(); }                \
}

