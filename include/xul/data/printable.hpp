#pragma once


namespace xul {


class output_stream;


/// pure interface, no reference-counting
class printable
{
public:
    virtual void print(output_stream* os) const = 0;
};


}
