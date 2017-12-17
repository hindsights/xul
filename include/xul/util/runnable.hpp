#pragma once

#include <xul/lang/object.hpp>


namespace xul {


class runnable : public object
{
public:
    virtual void run() = 0;
};


}
