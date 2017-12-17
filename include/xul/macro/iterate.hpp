#pragma once


#ifndef XUL_ITERATE
#define XUL_ITERATE(cont_type, cont, iter) for (boost::intrusive_ptr<xul::const_iterator<const cont_type::entry_type*> > (iter) = (cont)->iterate(); (iter)->available(); (iter)->next())
#endif

#ifndef XUL_ITERATE_REF
#define XUL_ITERATE_REF(cont_type, cont, iter) for (boost::intrusive_ptr<xul::iterator<cont_type::entry_type*> > (iter) = (cont)->iterate_ref(); (iter)->available(); (iter)->next())
#endif

