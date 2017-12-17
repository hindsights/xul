#pragma once

#include <xul/io/codec/message_decoder_adapter.hpp>
#include <xul/io/codec/decoder_buffer_message.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


class dummy_message_decoder : public object_impl<message_decoder>
{
public:
    typedef decoder_buffer_message decoder_message;

    virtual bool feed(const uint8_t* data, int size) { return true; }
    virtual void reset() { }
    virtual void abort() { }
    virtual const uint8_t* get_remaining_data() const { return NULL; }
    virtual int get_remaining_size() const { return 0; }
    virtual bool is_aborted() const { return true; }
    virtual void set_listener(message_decoder_listener* listener) { }

    static message_decoder* instsance()
    {
        static boost::intrusive_ptr<message_decoder> dummy_decoder = new dummy_message_decoder;
        return dummy_decoder.get();
    }
};


class direct_message_decoder : public message_decoder_adapter
{
public:
    typedef decoder_buffer_message decoder_message;

    virtual bool feed(const uint8_t* data, int size)
    {
        decoder_message msg(data, size);
        do_get_listener()->on_decoder_message(this, &msg);
        return true;
    }
    virtual bool is_aborted() const { return false; }
};


}
