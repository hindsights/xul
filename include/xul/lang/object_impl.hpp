#pragma once

#include <xul/lang/object_ptr.hpp>
#include <boost/detail/atomic_count.hpp>
#include <boost/checked_delete.hpp>
#include <boost/intrusive_ptr.hpp>
#include <assert.h>



namespace xul {


template <typename T>
class object_impl : public T
{
public:
    object_impl() : m_ref_count(0) { }
    virtual ~object_impl() { }

public:

    virtual int add_reference() const
    {
        assert(m_ref_count >= 0);
        ++m_ref_count;
        return m_ref_count;
    }
    virtual int release_reference() const
    {
        assert(m_ref_count > 0);
        int count = --m_ref_count;
        if (0 == count)
        {
            delete this;
        }
        return count;
    }
    virtual int get_reference_count() const
    {
        return m_ref_count;
    }

    boost::intrusive_ptr<T> self()
    {
        return boost::intrusive_ptr<T>(this);
    }
    boost::intrusive_ptr<const T> self() const
    {
        return boost::intrusive_ptr<const T>(this);
    }

    static T* create()
    {
        return new object_impl<T>;
    }

private:
    mutable boost::detail::atomic_count m_ref_count;
};


template <typename T>
inline T* create_object()
{
    return new object_impl<T>;
}


}
