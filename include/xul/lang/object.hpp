#pragma once


namespace xul {


class object
{
public:
    virtual int add_reference() const = 0;
    virtual int release_reference() const = 0;
    virtual int get_reference_count() const = 0;

protected:
    object() { }
    ~object() { }
private:
    object( const object& );
    const object& operator=( const object& );
};


}
