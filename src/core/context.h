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

#ifndef MEMPKO_DCI_CONTEXT_H
#define MEMPKO_DCI_CONTEXT_H

#include <algorithm>

#include "core/util.h"
#include "core/roles.h"
#include "core/dbc.h"

namespace mempko::muda::context
{ 
    template <class m>
        class modify_text_context 
        {
            public:
                modify_text_context(
                        role::modifiable_object<m, text_type>& md, 
                        const text_type& text) : 
                    _m{md}, _text{text} {}

                void operator()()
                {
                    _m.change(_text);
                }

            private:
                role::modifiable_object<m, text_type>& _m;
                text_type _text;
        };

    template <class m, class list_type>
        class set_incremental_id
        {
            public:
                set_incremental_id(
                        role::id_reciever<m>& md, 
                        const role::iterable<list_type>& list) :
                    _m{md}, _list{list} {}

                void operator()()
                {
                    //find max
                    using list_value = typename list_type::value_type;
                    auto cp = std::vector<list_value>{std::begin(_list), std::end(_list)};
                    auto max = std::max_element(
                            std::begin(cp), std::end(cp), 
                            [](const auto& a, const auto& b) 
                            {
                            REQUIRE(a); REQUIRE(b);
                            return a->id() < b->id();
                            });

                    //increment
                    auto id = max != cp.end() ? (*max)->id() + 1: 0;

                    //set
                    _m.recieve(id);                    
                } 

            private:
                role::id_reciever<m>& _m;
                const role::iterable<list_type>& _list;
        };

    template <class m, class m_ptr, class list>
        class add_object
        {
            public:
                add_object(m_ptr mp, list& l) : 
                    _m{mp}, _list{l} 
                {
                    REQUIRE(mp);
                    ENSURE(_m);
                }

                void operator()()
                {
                    using list_type = typename list::list_type;

                    auto mm = _m.modify();
                    auto set_id = set_incremental_id<m, list_type>{*mm, _list};

                    set_id();
                    mm->stamp();
                    _list.add(_m);
                }

            private:
                m_ptr _m;
                list& _list;
        };

    template<class container, class id>
        class remove_object
        {
            public:
                remove_object(
                        id v, 
                        role::removable<id>& removable) :
                    _id(v), _removable(removable) {}
                void operator()()
                {
                    const bool removed = _removable.remove(_id);
                    ENSURE(removed);
                }
            private:
                id _id;
                role::removable<id>& _removable;
        };

    class transition_state
    {
        public:
            transition_state(role::transitional_object& obj) : _obj(obj) {}
            void operator()()
            {
                _obj.transition();
            }

        private:
            role::transitional_object& _obj;
    };

}

#endif

