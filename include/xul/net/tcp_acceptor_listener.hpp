#pragma once



namespace xul {


class socket_address;
class inet_socket_address;
class tcp_socket;
class tcp_acceptor;


class tcp_acceptor_listener
{
public:
    virtual void on_acceptor_client(tcp_acceptor* acceptor, tcp_socket* newClient, const inet_socket_address& sockAddr) = 0;
    virtual void on_acceptor_error(tcp_acceptor* acceptor, int errcode) { }
};

class tcp_acceptor_handler:public object, public tcp_acceptor_listener
{
};


}
