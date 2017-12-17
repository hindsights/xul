#pragma once

#include <xul/std/istring_less.hpp>
#include <boost/intrusive_ptr.hpp>
#include <string>


namespace xul {


template <typename T>
class element_storage_traits : public element_traits<T>
{
public:
    typedef T storage_type;
    typedef typename element_traits<T>::accessor_type accessor_type;
    typedef typename element_traits<T>::const_accessor_type const_accessor_type;

    static accessor_type get_accessor(T val)
    {
        return val;
    }
    static const_accessor_type get_const_accessor(T val)
    {
        return val;
    }
    static T store(T v)
    {
        return v;
    }
};

template <typename ObjectT>
class element_storage_traits<ObjectT *> : public element_traits<ObjectT*>
{
public:
    typedef boost::intrusive_ptr<ObjectT> storage_type;
    typedef typename element_traits<ObjectT*>::accessor_type accessor_type;
    typedef typename element_traits<ObjectT*>::const_accessor_type const_accessor_type;

    static accessor_type get_accessor(const boost::intrusive_ptr<ObjectT>& val)
    {
        return val.get();
    }
    static const_accessor_type get_const_accessor(const boost::intrusive_ptr<const ObjectT>& val)
    {
        return val.get();
    }
    static storage_type store(ObjectT* s)
    {
        return storage_type(s);
    }
};


template <>
class element_storage_traits<const char*> : public element_traits<const char*>
{
public:
    typedef std::string storage_type;
    typedef element_traits<const char*>::accessor_type accessor_type;
    typedef element_traits<const char*>::const_accessor_type const_accessor_type;

    static accessor_type get_accessor(const std::string& val)
    {
        return val.c_str();
    }
    static const_accessor_type get_const_accessor(const std::string& val)
    {
        return val.c_str();
    }
    static std::string store(const char* s)
    {
        return std::string(s);
    }
};


template <typename T>
class key_element_storage_traits
{
public:
    typedef T accessor_type;
    typedef T const_accessor_type;
    typedef T input_type;

    typedef T storage_type;
    typedef std::less<T> comparer_type;

    static accessor_type get_accessor(T val)
    {
        return val;
    }
    static const_accessor_type get_const_accessor(T val)
    {
        return val;
    }
    static T store(T v)
    {
        return v;
    }
};


template <>
class key_element_storage_traits<const char*> : public element_storage_traits<const char*>
{
public:
    typedef std::less<std::string> comparer_type;
};

class istring_key_element_storage_traits : public element_storage_traits<const char*>
{
public:
    typedef istring_less<std::string> comparer_type;
};


}
