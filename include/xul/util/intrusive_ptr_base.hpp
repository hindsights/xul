#pragma once

#include <boost/detail/atomic_count.hpp>
#include <boost/checked_delete.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>


namespace xul {


template <typename T>
class intrusive_ptr_base : private boost::noncopyable
{
public:
    intrusive_ptr_base() : m_refs(0) {}

    friend void intrusive_ptr_add_ref(intrusive_ptr_base<T> const* s)
    {
        assert(s != 0);
        assert(s->m_refs >= 0);
        ++s->m_refs;
    }

    friend void intrusive_ptr_release(intrusive_ptr_base<T> const* s)
    {
        assert(s != 0);
        assert(s->m_refs > 0);
        if (--s->m_refs == 0)
        {
            boost::checked_delete(static_cast<T const*>(s));
        }
    }

    boost::intrusive_ptr<T> self()
    {
        return boost::intrusive_ptr<T>(static_cast<T*>(this));
    }

    boost::intrusive_ptr<const T> self() const
    {
        return boost::intrusive_ptr<const T>(static_cast<const T*>(this));
    }

    boost::intrusive_ptr<T> shared_from_this()
    {
        return boost::intrusive_ptr<T>(static_cast<T*>(this));
    }

    boost::intrusive_ptr<const T> shared_from_this() const
    {
        return boost::intrusive_ptr<const T>(static_cast<const T*>(this));
    }

    int refcount() const
    {
        return m_refs;
    }

private:
    /// reference counter for intrusive_ptr
    mutable boost::detail::atomic_count m_refs;
};


}
