#pragma once

#include <stddef.h>


namespace xul {


class data_input_stream;
class data_output_stream;


class serializable
{
public:
    virtual bool read_object( data_input_stream& is ) = 0;

    virtual void write_object( data_output_stream& os ) const = 0;

    //virtual int get_object_size() const = 0;
};


}
