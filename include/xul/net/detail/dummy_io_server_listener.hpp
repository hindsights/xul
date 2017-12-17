#pragma once

#include <xul/xio/io_server.hpp>
#include <assert.h>


namespace xul {
namespace detail {


class dummy_io_server_listener : public io_server_listener
{
public:
    virtual bool on_session_create(io_server* sender, io_session* session)
    {
        assert(false);
        return false;
    }
};


}
}
