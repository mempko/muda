/**
* Copyright (C) 2015  Maxim Noah Khailo
*
* This file is part of Muda.
* 
* Muda is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Muda is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
* 
* You should have received a copy of the GNU Affero General Public License
* along with Muda.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MEMPKO_MUDA_UTIL_H
#define MEMPKO_MUDA_UTIL_H

#include "core/types.h"
#include "core/dbc.h"

#include <algorithm>

namespace mempko::muda::util 
{ 
    template<class Vec, class Predicate>
        void filter(Vec& v, Predicate p)
        {
            v.erase(std::remove_if(v.begin(), v.end(),
                        [&](auto val) {return !p(val);}), v.end());
        }

}

#endif
