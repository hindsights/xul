#pragma once

#include <xul/xio/io_session.hpp>
#include <xul/xio/detail/dummy_io_session_listener.hpp>
#include <xul/io/codec/message_decoder_adapter.hpp>
#include <xul/lang/object_impl.hpp>
#include <xul/log/log.hpp>


namespace xul {


class io_session_adapter
    : public object_impl<io_session>
    , public message_decoder_listener
{
public:
    io_session_adapter()
    {
        set_listener(NULL);
        set_message_decoder(NULL);
    }

    virtual void close()
    {
        m_listener->on_session_close(this);
    }

    virtual void set_message_decoder(message_decoder* decoder)
    {
        XUL_LIBXUL_DEBUG("io_session.set_message_decoder " << xul::make_tuple(this, m_message_decoder.get(), decoder));
        m_message_decoder = decoder ? decoder : dummy_message_decoder::instsance();
        m_message_decoder->set_listener(this); // do not use shared_ptr here to avoid circular reference
    }
    virtual message_decoder* get_message_decoder()
    {
        return m_message_decoder.get();
    }
    virtual void set_listener(io_session_listener* listener)
    {
        m_listener = listener ? listener : &dummy_listener();
    }
    virtual void set_handler(io_session_handler* handler)
    {
        if (handler)
        {
            m_handler = handler;
            m_listener = m_handler.get();
        }
        else
        {
            m_listener = &dummy_listener();
        }
    }

    //message_decoder_ptr get_message_decoder() const    { return m_message_decoder; }



protected:
    static io_session_listener& dummy_listener()
    {
        static detail::dummy_io_session_listener the_listener;
        return the_listener;
    }
    message_decoder* do_get_message_decoder() const    { return m_message_decoder.get(); }

    virtual void attach_listener()
    {
    }

    virtual void open_session()
    {
        XUL_LIBXUL_DEBUG("io_session.open_session before " << xul::make_tuple(this, m_message_decoder.get()));
        //set_message_decoder(m_listener->create_message_decoder());
        do_get_message_decoder()->reset();
        XUL_LIBXUL_DEBUG("io_session.open_session end " << xul::make_tuple(this, m_message_decoder.get()));
        m_listener->on_session_open(this);
    }

protected:
    io_session_listener* do_get_listener()
    {
        return m_listener;
    }

private:
    io_session_listener* m_listener;
    boost::intrusive_ptr<io_session_handler> m_handler;
    boost::intrusive_ptr<message_decoder> m_message_decoder;
};


}
