#pragma once

#include <xul/lang/object.hpp>
#include <assert.h>


namespace boost {


inline void intrusive_ptr_add_ref(const xul::object* s)
{
    assert(s != 0);
    s->add_reference();
}


inline void intrusive_ptr_release(const xul::object* s)
{
    assert(s != 0);
    s->release_reference();
}

}

#include <boost/intrusive_ptr.hpp>

namespace xul {

typedef boost::intrusive_ptr<object> object_ptr;

}
