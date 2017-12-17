#pragma once

#include <algorithm>


namespace xul {


class containers
{
public:
    template <typename ContainerT>
    static bool contains(const ContainerT& container, const typename ContainerT::key_type& key)
    {
        return container.find(key) != container.end();
    }
};

class sequences
{
public:
    template <typename InputIteratorT, typename ValueT>
    static bool contains(InputIteratorT first, InputIteratorT last, const ValueT& val)
    {
        return std::find(first, last, val) != last;
    }
    template <typename ContainerT>
    static bool contains(const ContainerT& container, const typename ContainerT::value_type& val)
    {
        return sequences::contains(container.begin(), container.end(), val);
    }

    template <typename ContainerT>
    static typename ContainerT::const_iterator find(const ContainerT& cont, const typename ContainerT::value_type& val)
    {
        return std::find(cont.begin(), cont.end(), val);
    }

    template <typename OutputIteratorT, typename InputIteratorT>
    static void copy(InputIteratorT first, size_t size, OutputIteratorT dest)
    {
        for (size_t index = 0; index < size; ++index)
        {
            *dest = *first;
            ++first;
            ++dest;
        }
    }

    template <typename ContainerT>
    static void append( ContainerT& target, const ContainerT& src )
    {
        target.insert( target.end(), src.begin(), src.end() );
    }
    
    template <typename ContainerT>
    static void extend( ContainerT& target, const ContainerT& src )
    {
        target.insert( target.end(), src.begin(), src.end() );
    }

    template <typename ContainerT>
    static typename ContainerT::value_type pop(ContainerT& container)
    {
        assert(false == container.empty());
        typename ContainerT::value_type val = container.top();
        container.pop();
        return val;
    }

    template <typename ContainerT>
    static typename ContainerT::value_type pop_front(ContainerT& container)
    {
        assert(false == container.empty());
        typename ContainerT::value_type val = container.front();
        container.pop_front();
        return val;
    }
};


class arrays
{
public:
    template <typename OutputIteratorT, typename RandomInputIteratorT>
    static void copy(RandomInputIteratorT src, size_t size, OutputIteratorT dest)
    {
        std::copy(src, src + size, dest);
    }

    template <typename RandomInputIteratorT, typename ValueT>
    static RandomInputIteratorT find(RandomInputIteratorT first, size_t size, const ValueT& val)
    {
        return std::find(first, first + size, val);
    }

    template <typename RandomInputIteratorT, typename ValueT>
    static bool contains(RandomInputIteratorT first, size_t size, const ValueT& val)
    {
        return sequences::contains(first, first + size, val);
    }

    template <typename T, size_t N>
    static size_t count_of( T (&)[N] ) { return N; }
};

}



