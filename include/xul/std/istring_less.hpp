#pragma once

#include <xul/std/strings.hpp>


namespace xul {


template <typename StringT>
struct istring_less
{
    bool operator()( const StringT& x, const StringT& y ) const
    {
        return strings::to_lower_copy(x) < strings::to_lower_copy(y);
    }
};


}
