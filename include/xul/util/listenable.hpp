#pragma once


/**
 * @file
 * @brief event listenable
 */


namespace xul {


/// listable object, managing one external listener
template <typename ListenerT>
class listenable
{
public:
    virtual void set_listener(ListenerT* listener) = 0;
    virtual void reset_listener() = 0;
};


}
