#pragma once

#include <xul/util/runnable.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


template <typename CallbackT>
class runnable_callback : public object_impl<runnable>
{
public:

    explicit runnable_callback(const CallbackT& callback) : m_callback(callback) { }

    virtual void run()
    {
        m_callback();
    }

private:
    CallbackT m_callback;
};


typedef boost::intrusive_ptr<runnable> runnable_ptr;

template <typename CallbackT>
inline runnable_ptr make_runnable(const CallbackT& callback)
{
    return runnable_ptr(new runnable_callback<CallbackT>(callback));
}


}
