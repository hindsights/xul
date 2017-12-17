#pragma once

//#include <xul/net/tcp_socket.hpp>
#include <xul/lang/object.hpp>
#include <stdint.h>


namespace xul {


class tcp_socket;
class url_response;
class url_session;


class url_session_listener
{
public:
    virtual void on_session_close(url_session* session) = 0;
};


/// handles a single url request from client
class url_session : public object
{
public:
    virtual void close() = 0;
    virtual void finish() = 0;
    virtual void reset() = 0;
    virtual tcp_socket* get_connection() = 0;
    virtual url_response* get_response() = 0;

    virtual bool send_header() = 0;
    virtual bool send_data(const uint8_t* data, int size) = 0;

    virtual bool is_chunked() const = 0;
    virtual void set_chunked(bool enabled) = 0;

    virtual bool is_finished() const = 0;
    virtual bool is_keep_alive() const = 0;
    virtual void set_keep_alive(bool keep_alive) = 0;

    virtual void register_listener(url_session_listener* listener) = 0;
    virtual void unregister_listener(url_session_listener* listener) = 0;
};


}
