#pragma once

#include <list>

namespace xul {

class lists
{
public:
    template <typename T>
    static void append( std::list<T>& target, const std::list<T>& src )
    {
        target.insert( target.end(), src.begin(), src.end() );
    }

    template <typename T>
    static void limit_max( std::list<T>& target, size_t maxCount )
    {
        if ( target.size() > maxCount )
        {
            target.resize( maxCount );
        }
    }
};

}

