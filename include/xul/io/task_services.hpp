#pragma once

#include <xul/io/task_service.hpp>
#include <xul/util/runnable_callback.hpp>


namespace xul {


class task_services
{
public:
    template <typename CallbackT>
    static void post(task_service& svc, const CallbackT& callback)
    {
        boost::intrusive_ptr<runnable> r = make_runnable(callback);
        svc.post(r.get());
    }
};

}
