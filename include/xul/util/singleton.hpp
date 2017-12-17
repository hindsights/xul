#pragma once


namespace xul {


template <typename T>
class singleton
{
public:
    typedef T instance_type;

    static T& instance()
    {
        static T the_instance;
        return the_instance;
    }
};


}
