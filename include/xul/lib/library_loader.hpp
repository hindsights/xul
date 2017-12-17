#pragma once


namespace xul {


class library_loader : public object
{
public:
    virtual library_repository* get_repository() = 0;
};


}
