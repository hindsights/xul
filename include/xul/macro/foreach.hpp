#pragma once


/// 'foreach' for mutable stl containers
#ifndef XUL_FOREACH
#define XUL_FOREACH(containerType, container, iter)    \
    for ( containerType::iterator iter = (container).begin(), the_end = (container).end(); (iter) != (the_end); ++(iter) )
#endif

/// 'foreach' for immutable stl containers
#ifndef XUL_FOREACH_CONST
#define XUL_FOREACH_CONST(containerType, container, iter)    \
    for ( containerType::const_iterator iter = (container).begin(), the_end = (container).end(); (iter) != (the_end); ++(iter) )
#endif

