#pragma once

#include <xul/xio/io_session_listener.hpp>
#include <xul/io/codec/dummy_message_decoder.hpp>
#include <assert.h>


namespace xul {
namespace detail {


class dummy_io_session_listener : public io_session_listener
{
public:
    virtual message_decoder* create_message_decoder()
    {
        static boost::intrusive_ptr<message_decoder> dummy_decoder = new dummy_message_decoder;
        return dummy_decoder.get();
    }
};


}
}
