#pragma once


namespace xul {


/// basic bsd socket
class socket_option
{
public:
    virtual int get_command() const = 0;
    virtual char* get_data() const = 0;
};


}
