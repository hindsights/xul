#pragma once


namespace xul {


template <typename T>
class element_traits
{
public:
    typedef T accessor_type;
    typedef T const_accessor_type;
    typedef T input_type;
};

template <typename ObjectT>
class element_traits<ObjectT *>
{
public:
    typedef ObjectT *accessor_type;
    typedef ObjectT const *const_accessor_type;
    typedef ObjectT *input_type;
};


template <>
class element_traits<const char*>
{
public:
    typedef const char* accessor_type;
    typedef const char* const_accessor_type;
    typedef const char* input_type;
};


}
