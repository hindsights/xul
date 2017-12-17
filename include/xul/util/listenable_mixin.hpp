#pragma once

/**
 * @file
 * @brief event listenable mixin
 */

#include <xul/util/listenable.hpp>
//#include <boost/shared_ptr.hpp>


namespace xul {


/// listable mixin, managing one external listener
template <typename BaseT, typename ListenerT, typename DummyListenerT = ListenerT, typename CheckedListenerT = DummyListenerT>
class listenable_mixin : public BaseT
{
public:
    typedef ListenerT listener_type;
    typedef DummyListenerT dummy_listener_type;
    typedef CheckedListenerT checked_listener_type;

    listenable_mixin()
    {
        set_listener(checked_listener());
    }
    virtual ~listenable_mixin()
    {
    }

    virtual void set_listener(listener_type* listener)
    {
        // null-object pattern, simplifying listener callbacking
        m_listener = listener ? listener : dummy_listener();
        //m_listener_ref.reset(); // release old listener reference
    }
    //virtual void set_listener(boost::shared_ptr<listener_type> listener)
    //{
    //    m_listener = (listener) ? listener.get() : dummy_listener();
    //}
#if 1
    virtual void reset_listener()
    {
        set_listener(dummy_listener());
    }
#endif

    /// maybe virtual
    //listener_type* get_listener() const    { return m_listener; }

    static listener_type* checked_listener()
    {
        static checked_listener_type theListener;
        return &theListener;
    }
    static listener_type* dummy_listener()
    {
        static dummy_listener_type theListener;
        return &theListener;
    }

protected:
    /// non-virtual version, used internally
    listener_type* do_get_listener() const    { return m_listener; }
    //boost::shared_ptr<listener_type> do_get_listener_ref() const    { return m_listener_ref; }

private:
    listener_type* m_listener;
    //boost::shared_ptr<listener_type> m_listener_ref;
};


template <typename BaseT, typename ListenerT, typename DummyListenerT = ListenerT, typename CheckedListenerT = DummyListenerT>
class closable_listenable_mixin : public listenable_mixin<BaseT, ListenerT, DummyListenerT, CheckedListenerT>
{
public:
    virtual void destroy()
    {
        this->reset_listener();
        this->close();
    }
};


}
