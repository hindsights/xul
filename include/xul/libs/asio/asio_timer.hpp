#pragma once

#include <xul/util/timer.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/util/detail/dummy_timer_listener.hpp>
#include <xul/net/asio/io_service_provider.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <xul/macro/minmax.hpp>
#include <xul/log/log.hpp>

#if defined(XUL_TIMER_USE_TICK_COUNT)
#include <xul/libs/asio/tick_count_deadline_timer.hpp>
#elif defined(XUL_TIMER_USE_POSIX_TIME)
#include <xul/libs/asio/posix_deadline_timer.hpp>
#else
#include <xul/libs/asio/time_counter_deadline_timer.hpp>
#endif
#include <xul/util/intrusive_shared_from_this.hpp>

#include <boost/asio/placeholders.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace xul {


class asio_timer
    : public listenable_mixin<object_impl<timer>, timer_listener, detail::dummy_timer_listener, detail::checked_timer_listener>
    , public intrusive_shared_from_this<asio_timer>
{
private:
    class boost_function_callback : public timer_listener
    {
    public:
        explicit boost_function_callback(const boost::function0<void>& func) : m_func(func) { }
        virtual void on_timer_elapsed(timer* sender)
        {
            m_func();
        }
    private:
        boost::function0<void> m_func;
    };

    //static boost::shared_ptr<boost_function_callback> bind_callback(const boost::function0<void>& func)
    //{
    //    return boost::shared_ptr<boost_function_callback>(new boost_function_callback(func));
    //}
public:
    typedef xul::detail::timer_traits timer_traits_type;
    typedef timer_traits_type::time_traits_type time_traits_type;
    typedef time_traits_type::time_type time_type;
    typedef boost::asio::basic_deadline_timer<time_type, time_traits_type> native_type;
    typedef timer_traits_type::duration_type time_duration_type;


    explicit asio_timer(boost::asio::io_service& inIOSerivce)
        : m_native(inIOSerivce), m_started(false), m_times( 0 ), m_interval( 0 )
    {
        XUL_LOGGER_INIT("timer");
        XUL_DEBUG("new");
    }
    virtual ~asio_timer()
    {
        XUL_DEBUG("delete");
        assert(false == is_started());
    }
    //virtual void set_callback(const boost::function0<void>& callback)
    //{
    //    set_listener(bind_callback(callback));
    //}
    virtual void reset_callback()
    {
        reset_listener();
    }

    int get_interval() const { return m_interval; }

    int64_t get_times() const { return m_times; }

    bool start(int interval)
    {
        this->stop();
        m_interval = interval;
        if (false == do_start(interval))
            return false;
        m_started = true;
        return true;
    }

    void stop()
    {
        XUL_DEBUG("stop " << m_started);
        m_started = false;
        this->do_stop();
    }

    bool is_started() const { return m_started; }

    native_type& get_native() { return m_native; }
    const native_type& get_native() const { return m_native; }

protected:
    virtual bool do_start(int interval)
    {
        //    TRACE("OnceTimer::Start %p %d\n", this, interval);
        this->start_from_now(interval);
        return true;
    }

    virtual void do_stop()
    {
        cancel_timer();
    }

    virtual void on_timer()
    {
        notify();
    }

    void notify()
    {
        do_get_listener()->on_timer_elapsed(this);
    }


protected:
    void cancel_timer()
    {
        //UTIL_ERROR("timer_impl::stop " << make_tuple(this, m_started, m_detached));
        get_native().cancel(m_last_error);
    }
    void start_from_now(int interval)
    {
        //assert(interval > 1);
        //UTIL_ERROR("timer_impl::start_from_now " << make_tuple(this, m_started, m_detached, interval));
        get_native().expires_from_now(time_duration_type(interval), m_last_error);
        this->async_wait();
    }
    void start_at(const native_type::time_type& expireTime)
    {
        //UTIL_ERROR("timer_impl::start_at " << make_tuple(this, m_started, m_detached));
        get_native().expires_at(expireTime, m_last_error);
        this->async_wait();
    }
    void async_wait()
    {
        start_wait();
    }
    void on_timer_callback()
    {
        //UTIL_ERROR("timer_impl::on_timer_message " << make_tuple(this, m_started, m_detached, err.value()));
        on_timer();
    }

    void start_wait()
    {
        m_native.async_wait(
            boost::bind(
            &asio_timer::on_timer_impl_callback,
            this->shared_from_this(),
            boost::asio::placeholders::error
            )
            );
    }

    void on_timer_impl_callback(const boost::system::error_code& err)
    {
        if ( err )
        {
            assert( boost::asio::error::operation_aborted == err );
            return;
        }
        on_timer_callback();
    }

protected:
    XUL_LOGGER_DEFINE();
    native_type m_native;
    boost::system::error_code m_last_error;
    bool m_started;
    int m_interval;
    int64_t m_times;
};


class asio_once_timer : public asio_timer
{
public:
    explicit asio_once_timer(boost::asio::io_service& inIOSerivce = io_service_provider::default_io_service())
        : asio_timer(inIOSerivce)
    {
    }
    virtual ~asio_once_timer()
    {
        this->stop();
    }

    int64_t get_elapsed_time() const { return 0; }
};


class asio_periodic_timer : public asio_timer
{
public:
    explicit asio_periodic_timer(boost::asio::io_service& inIOSerivce = io_service_provider::default_io_service())
        : asio_timer(inIOSerivce)
    {
    }
    virtual ~asio_periodic_timer()
    {
        this->stop();
    }

    int64_t get_elapsed_time() const { return m_times * m_interval; }

protected:
    virtual bool do_start(int interval)
    {
        assert(interval >= 0);
        XUL_LIMIT_MIN(interval, 0);
        //    TRACE("PeriodicTimer::Start %p %d\n", this, interval);
        //    m_starttime = timer_impl_type::traits_type::now();
        m_firetime = native_type::traits_type::now();
        m_times = 0;
        this->do_start_once();
        return true;
    }

    virtual void on_timer()
    {
        //XULTRACE("on_timer 0x%p\n", this);
        if (false == m_started)
        {
        //    assert(m_started);
            return;
        }
        ++m_times;
        this->do_start_once();

        notify();
    }

    void do_start_once()
    {
        //XULTRACE("start timer %u %u 0x%p\n", m_interval, m_firetime, this);
        m_firetime = native_type::traits_type::add(m_firetime, time_duration_type(m_interval));
        this->start_at(m_firetime);
    }

protected:
    native_type::time_type m_firetime;
};


}
