#pragma once

#include <boost/intrusive_ptr.hpp>


namespace xul {


template <typename T>
class intrusive_shared_from_this
{
public:
    intrusive_shared_from_this() {}

    boost::intrusive_ptr<T> shared_from_this()
    {
        return boost::intrusive_ptr<T>(static_cast<T*>(this));
    }

    boost::intrusive_ptr<const T> shared_from_this() const
    {
        return boost::intrusive_ptr<const T>(static_cast<const T*>(this));
    }

};


}
