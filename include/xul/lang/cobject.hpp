#pragma once


#define XUL_DECLARE_COBJECT(name)    \
    class name; \
    long name##_add_ref(const name* obj); \
    long name##_release_ref(const name* obj); \
    long name##_get_ref_count(const name* obj); \
    long cobject_add_ref(const name* obj); \
    long cobject_release_ref(const name* obj)

