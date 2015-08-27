#ifndef MEMPKO_DCI_H
#define MEMPKO_DCI_H

#include <iostream>
#include <boost/assert.hpp>

namespace mempko { namespace dci {

    template <class concrete_derived>
        class methodful_role 
        {
            protected:
                const concrete_derived* self() const { return static_cast<const concrete_derived*>(this);}
                concrete_derived* self() { return static_cast<concrete_derived*>(this);}
        };

}}//namespace

#endif
