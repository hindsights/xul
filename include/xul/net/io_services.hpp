#pragma once

#include <xul/net/io_service.hpp>
#include <xul/util/runnable_callback.hpp>


namespace xul {


class tcp_socket;
class udp_socket;
class tcp_acceptor;
class io_task_service;
class name_resolver;
class timer;


class io_services
{
public:
    template <typename CallbackT>
    static void post_runnable(io_service* ios, CallbackT callback)
    {
        boost::intrusive_ptr<runnable> r = make_runnable(callback);
        ios->post(r.get());
    }
};


}
