#pragma once

#include <xul/net/uri.hpp>
#include <xul/lib/library_manager.hpp>
#include <xul/lang/object_ptr.hpp>
#include <string>


namespace xul {


/// interface for uri
class uris
{
public:
    static boost::intrusive_ptr<uri> parse(const char* urlstr)
    {
        boost::intrusive_ptr<uri> u = create_uri();
        if (!u)
            return u;
        if (u->parse(urlstr))
            return u;
        return boost::intrusive_ptr<uri>();
    }
    static boost::intrusive_ptr<uri> parse(const std::string& urlstr)
    {
        return parse(urlstr.c_str());
    }
};


}
