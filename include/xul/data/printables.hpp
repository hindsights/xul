#pragma once

#include <xul/data/printable.hpp>
#include <xul/io/ostream_stream.hpp>


namespace xul {


inline std::ostream& operator<<(std::ostream& os, const printable& obj)
{
    xul::ostream_stream oss(os);
    obj.print(&oss);
    return os;
}


class printables
{
public:
    static std::string to_string(const printable& obj)
    {
        std::ostringstream os;
        os << obj;
        return os.str();
    }
};


}
