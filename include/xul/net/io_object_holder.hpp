#pragma once

#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>


namespace xul {


template <typename IOObjectT>
class io_object_holder : private boost::noncopyable
{
    typedef io_object_holder<IOObjectT> this_type;
public:
    typedef IOObjectT io_object_type;
    typedef boost::shared_ptr<IOObjectT> io_object_ptr_type;

    io_object_holder()
    {
    }
    explicit io_object_holder(io_object_ptr_type obj) : m_io_object(obj)
    {
    }
    explicit io_object_holder(io_object_type* obj) : m_io_object(obj)
    {
    }
    ~io_object_holder()
    {
        if (m_io_object)
        {
            m_io_object->reset_listener();
            m_io_object->close();
            m_io_object.reset();
        }
    }
    operator bool() const
    {
        return m_io_object;
    }
    io_object_ptr_type get() const // never throws
    {
        return m_io_object.get();
    }

    void reset(io_object_ptr_type p = io_object_ptr_type()) // never throws
    {
        assert( !p || p != m_io_object ); // catch self-reset errors
        this_type(p).swap(*this);
    }

    void reset(io_object_type* p) // never throws
    {
        assert( !p || p != m_io_object.get() ); // catch self-reset errors
        this_type(p).swap(*this);
    }

    io_object_type& operator*() const // never throws
    {
        assert( m_io_object );
        return *m_io_object;
    }

    io_object_type* operator->() const // never throws
    {
        assert( m_io_object );
        return m_io_object.get();
    }

    void swap(io_object_holder& b) // never throws
    {
        io_object_ptr_type tmp = b.m_io_object;
        b.m_io_object = m_io_object;
        m_io_object = tmp;
    }
protected:
    io_object_ptr_type m_io_object;
};


}
