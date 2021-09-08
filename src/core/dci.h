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

#ifndef MEMPKO_DCI_H
#define MEMPKO_DCI_H

#include <iostream>
#include <boost/assert.hpp>

namespace mempko::dci
{ 
#define ADD_SELF(t) \
    t* const self() { return static_cast<t*>(this);} \
    const t* self() const { return static_cast<const t*>(this);} 

#define TAG(t) struct t {};
}

#endif
