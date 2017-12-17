#pragma once

#include <xul/io/task_service.hpp>


namespace xul {


class tcp_socket;
class udp_socket;
class tcp_acceptor;
class io_task_service;
class name_resolver;
class timer;
class message_socket;


class io_service : public task_service
{
public:
    virtual name_resolver* create_name_resolver() = 0;
    virtual udp_socket* create_udp_socket() = 0;
    virtual tcp_socket* create_tcp_socket() = 0;
    virtual tcp_acceptor* create_tcp_acceptor() = 0;
    virtual timer* create_once_timer() = 0;
    virtual timer* create_periodic_timer() = 0;
    virtual void* get_native() = 0;
    /// recreate udp socket object from native handle
    virtual udp_socket* recreate_udp_socket(int handle) = 0;
    /// recreate tcp socket object from native handle
    virtual tcp_socket* recreate_tcp_socket(int handle, bool connected) = 0;
    /// recreate tcp acceptor object from native handle
    virtual tcp_acceptor* recreate_tcp_acceptor(int handle) = 0;
    virtual message_socket* recreate_message_socket(int handle) = 0;
};


io_service* create_io_service();


}
