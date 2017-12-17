#pragma once

#include <xul/lang/cobject.hpp>
#include <boost/config.hpp>


namespace xul {


struct cobject;
long cobject_add_ref(const cobject* obj);
long cobject_release_ref(const cobject* obj);
long cobject_get_ref_count(const cobject* obj);


template <typename T>
class cobject_ptr
{
private:
    typedef cobject_ptr this_type;

public:
    typedef T element_type;

    cobject_ptr(): px( 0 )
    {
    }

    cobject_ptr(T * p, bool add_ref = true): px(p)
    {
        if (px != 0 && add_ref)
            cobject_add_ref(px);
    }

#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)
    template<class U>
#if !defined( BOOST_SP_NO_SP_CONVERTIBLE )
    cobject_ptr(cobject_ptr<U> const & rhs, typename boost::detail::sp_enable_if_convertible<U,T>::type = boost::detail::sp_empty())
#else
    cobject_ptr(cobject_ptr<U> const & rhs)
#endif
        : px(rhs.get())
    {
        if (px != 0)
            cobject_add_ref(px);
    }
#endif

    cobject_ptr(cobject_ptr const & rhs): px(rhs.px)
    {
        if ( px != 0 )
            cobject_add_ref(px);
    }

    ~cobject_ptr()
    {
        if ( px != 0 )
            cobject_release_ref(px);
    }

#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)

    template<class U> cobject_ptr & operator=(cobject_ptr<U> const & rhs)
    {
        cobject_ptr(rhs).swap(*this);
        return *this;
    }

#endif

    // Move support

#if defined( BOOST_HAS_RVALUE_REFS )

    cobject_ptr(cobject_ptr && rhs): px( rhs.px )
    {
        rhs.px = 0;
    }

    cobject_ptr & operator=(cobject_ptr && rhs)
    {
        this_type( static_cast< cobject_ptr && >( rhs ) ).swap(*this);
        return *this;
    }

#endif

    cobject_ptr & operator=(cobject_ptr const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    cobject_ptr & operator=(T * rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    void reset()
    {
        this_type().swap( *this );
    }

    void reset( T * rhs )
    {
        this_type( rhs ).swap( *this );
    }

    T * get() const
    {
        return px;
    }
    operator T*() const
    {
        return px;
    }

    T & operator*() const
    {
        BOOST_ASSERT( px != 0 );
        return *px;
    }

    T * operator->() const
    {
        BOOST_ASSERT( px != 0 );
        return px;
    }

    // implicit conversion to "bool"
//#include <boost/smart_ptr/detail/operator_bool.hpp>

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const // never throws
    {
        return px == 0? 0: &this_type::px;
    }
    // operator! is redundant, but some compilers need it
    bool operator! () const // never throws
    {
        return px == 0;
    }

#if 1
    operator bool() const
    {
        return !!px;
    }
#endif

    void swap(cobject_ptr & rhs)
    {
        T * tmp = px;
        px = rhs.px;
        rhs.px = tmp;
    }

private:
    T * px;
};


}
