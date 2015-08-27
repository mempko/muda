#ifndef MEMPKO_DCI_H
#define MEMPKO_DCI_H

#include <iostream>
#include <boost/assert.hpp>

namespace mempko { namespace dci {

#define ADD_SELF(t) \
    t* const self() { return static_cast<t*>(this);} \
    const t* self() const { return static_cast<const t*>(this);} 

#define TAG(t) struct t {};


}}//namespace

#endif
