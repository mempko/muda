/*
 * Copyright (C) 2012  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
