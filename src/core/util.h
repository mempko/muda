#ifndef MEMPKO_MUDA_UTIL_H
#define MEMPKO_MUDA_UTIL_H

#include "core/types.h"

namespace mempko { namespace muda { namespace util {

    template<class ptr_has_id>
        struct comp_ptr_by_id
        {
            bool operator()(const ptr_has_id m1, const ptr_has_id m2) 
            {
                BOOST_ASSERT(m1);
                BOOST_ASSERT(m2);
                return m1->id() < m2->id();
            }
        };
}}}//namespace

#endif
