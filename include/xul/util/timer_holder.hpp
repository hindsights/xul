#pragma once

#include <xul/util/timer.hpp>
#include <xul/net/io_service.hpp>
#include <xul/lang/object_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>


namespace xul {


class io_service;


class timer_holder : private boost::noncopyable
{
private:
    class timer_listener_object : public timer_listener
    {
    public:
        virtual ~timer_listener_object() { }
    };
    template <typename CallbackT>
    class callback_listener : public timer_listener_object
    {
    public:
        explicit callback_listener(const CallbackT& callback) : m_callback(callback) { }
        virtual void on_timer_elapsed(timer* sender)
        {
            m_callback();
        }
    private:
        CallbackT m_callback;
    };

    template <typename CallbackT>
    static boost::shared_ptr<timer_listener_object> bind_callback(const CallbackT& callback)
    {
        return boost::shared_ptr<timer_listener_object>(new callback_listener<CallbackT>(callback));
    }
public:
    timer_holder() { }
    explicit timer_holder(timer* a_timer) : m_timer(a_timer)
    {
    }
    ~timer_holder()
    {
        if (m_timer)
        {
            m_timer->reset_listener();
            m_timer->stop();
            m_timer.reset();
        }
    }
    bool is_valid() const
    {
		return m_timer.get() != NULL;;
    }
    void close()
    {
        m_timer->reset_listener();
        m_timer->stop();
    }

    void create_once_timer(io_service* ios)
    {
        m_timer = ios->create_once_timer();
    }

    void create_periodic_timer(io_service* ios)
    {
        m_timer = ios->create_periodic_timer();
    }

    void set_listener(timer_listener* listener)
    {
        if (!m_timer)
        {
            assert(false);
            return;
        }
        m_timer->set_listener(listener);
    }
    void reset_listener()
    {
        if (!m_timer)
        {
            assert(false);
            return;
        }
        m_timer->reset_listener();
    }
    template <typename CallbackT>
    void set_callback(const CallbackT& callback)
    {
        m_callback = bind_callback(callback);
        set_listener(m_callback.get());
    }
    void reset_callback()
    {
        this->reset_listener();
    }

    bool start(int interval)
    {
        if (!m_timer)
        {
            assert(false);
            return false;
        }
        return m_timer->start(interval);
    }
    void stop()
    {
        if (!m_timer)
        {
            assert(false);
            return;
        }
        m_timer->stop();
    }
    bool is_started() const
    {
        if (!m_timer)
        {
            assert(false);
            return false;
        }
        return m_timer->is_started();
    }
    int get_interval() const
    {
        if (!m_timer)
        {
            assert(false);
            return 0;
        }
        return m_timer->get_interval();
    }
    int64_t get_times() const
    {
        if (!m_timer)
        {
            assert(false);
            return 0;
        }
        return m_timer->get_times();
    }
    int64_t get_elapsed_time() const
    {
        if (!m_timer)
        {
            assert(false);
            return 0;
        }
        return m_timer->get_elapsed_time();
    }

private:
    boost::intrusive_ptr<timer> m_timer;
    boost::shared_ptr<timer_listener_object> m_callback;
};


}
