#pragma once

#include <xul/io/task_service.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/util/runnable.hpp>
#include <xul/util/ptime.hpp>
#include <xul/log/log.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>


namespace xul {


class thread_task_service : public object_impl<task_service>
{
public:
    thread_task_service()
    {
        XUL_LOGGER_INIT("thread_task_service");
    }

    virtual void start()
    {
        stop();
        m_thread.reset(new boost::thread(boost::bind(&thread_task_service::run, boost::intrusive_ptr<thread_task_service>(this))));
    }
    virtual void stop()
    {
        if (m_thread)
        {
            m_thread->join();
            m_thread.reset();
        }
    }
    virtual void stop(unsigned long milliseconds)
    {
        if (m_thread)
        {
            m_thread->timed_join(boost::posix_time::milliseconds(milliseconds));
            m_thread.reset();
        }
    }
    virtual void wait()
    {
        if (m_thread)
            m_thread->join();
    }
    virtual void post(runnable* task)
    {
        XUL_DEBUG("post task " << task);
        {
            boost::mutex::scoped_lock lock(m_mutex);
            m_tasks.push_back(boost::intrusive_ptr<runnable>(task));
        }
        m_condition.notify_one();
    }

protected:
    void run()
    {
        for (;;)
        {
            boost::intrusive_ptr<runnable> task = pop_or_wait();
            if (!task)
                break;
            XUL_DEBUG("run task " << task.get());
            task->run();
            break;
        }
    }
    boost::intrusive_ptr<runnable> pop_or_wait()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        while(m_tasks.empty())
        {
            m_condition.wait(lock);
        }
        boost::intrusive_ptr<runnable> task = m_tasks.front();
        m_tasks.pop_front();
        return task;
    }

private:
    XUL_LOGGER_DEFINE();
    boost::shared_ptr<boost::thread> m_thread;
    std::list<boost::intrusive_ptr<runnable> > m_tasks;
    boost::condition_variable m_condition;
    mutable boost::mutex m_mutex;
};


}
