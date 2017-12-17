#pragma once

#include <boost/detail/atomic_count.hpp>


namespace xul {


class atomic_count
{
public:
    atomic_count() : m_count(0)
    {
    }
#if 1
    long operator++()
    {
        return ++m_count;
    }

    long operator--()
    {
        return --m_count;
    }

    operator long() const
    {
        return m_count;
    }
#endif
private:
    boost::detail::atomic_count m_count;
};


}


#define XUL_IMPLENEBT_COBJECT_REF(name)    \
    long name##_add_ref(const name* obj) { return ++(obj->refcount); } \
    long name##_release_ref(const name* obj) { --(obj->refcount); long count = obj->refcount; if (count <= 0) { delete obj; } return count; } \
    long name##_get_ref_count(const name* obj) { return obj->refcount; }; \
    long cobject_add_ref(const name* obj) { return name##_add_ref(obj); } \
    long cobject_release_ref(const name* obj) { return name##_release_ref(obj); }

#define XUL_COBJECT_HEADER() \
    mutable xul::atomic_count refcount

