#pragma once

#include <xul/io/serializable.hpp>
#include <string>
#include <stdint.h>


namespace xul {


class socks4_request : public xul::serializable
{
public:
    enum { max_data_size = 64 };

    uint8_t version_number;
    uint8_t command_code;
    uint16_t destination_port;
    uint32_t destination_ip;
    std::string user_id;
    //std::string domain_name;

    socks4_request()
    {
        reset();
    }
    void reset()
    {
        version_number = 0;
        command_code = 0;
        destination_ip = 0;
        destination_port = 0;
        user_id.clear();
        //domain_name.clear();
    }

    virtual bool read_object( xul::data_input_stream& is )
    {
        reset();
        if (!is.is_available(9))
        {
            is.set_bad();
            return false;
        }
        is >> version_number >> command_code >> destination_port >> destination_ip;
        if (!is)
            return false;
        uint8_t ch;
        while (is.read_byte(ch) && ch > 0)
        {
            user_id.push_back(ch);
        }
        return is.good();
    }

    virtual size_t get_object_size() const
    {
        return 9 + user_id.size();
    }

    virtual void write_object( xul::data_output_stream& os ) const
    {
        os << version_number << command_code << destination_port << destination_ip;
        os.write_bytes(user_id.data(), user_id.size());
        os.write_byte(0);
    }

};

class socks4_response : public xul::serializable
{
public:
    uint8_t status_code;
    uint16_t destination_port;
    uint32_t destination_ip;

    static const int object_size = 8;

    socks4_response()
    {
        reset();
    }
    void reset()
    {
        status_code = 0;
        destination_ip = 0;
        destination_port = 0;
    }

    virtual bool read_object( xul::data_input_stream& is )
    {
        uint8_t null_byte = 0;
        is >> null_byte >> status_code >> destination_port >> destination_ip;
        if (null_byte != 0)
            is.set_bad();
        return is.good();
    }

    virtual size_t get_object_size() const
    {
        return 8;
    }

    virtual void write_object( xul::data_output_stream& os ) const
    {
        os.write_byte(0);
        os << status_code << destination_port << destination_ip;
    }

};


}
