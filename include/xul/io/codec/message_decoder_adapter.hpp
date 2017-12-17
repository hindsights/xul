#pragma once

#include <xul/io/codec/message_decoder.hpp>
#include <xul/io/codec/detail/dummy_message_decoder_listener.hpp>
#include <xul/lang/object_impl.hpp>


namespace xul {


class message_decoder_adapter : public object_impl<message_decoder>
{
public:
    message_decoder_adapter()
    {
        set_listener(NULL);
    }
    virtual bool feed(const uint8_t* data, int size) { return true; }
    virtual void reset() { }
    virtual void abort() { }
    virtual const uint8_t* get_remaining_data() const { return NULL; }
    virtual int get_remaining_size() const { return 0; }
    virtual bool is_aborted() const { return true; }
    virtual void set_listener(message_decoder_listener* listener)
    {
        m_listener = listener ? listener : dummy_listener();
    }

protected:
    static message_decoder_listener* dummy_listener()
    {
        static detail::dummy_message_decoder_listener the_listener;
        return &the_listener;
    }
    message_decoder_listener* do_get_listener()
    {
        return m_listener;
    }

private:
    message_decoder_listener* m_listener;
};


}
