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

#ifndef MEMPKO_DCI_CONTEXT_H
#define MEMPKO_DCI_CONTEXT_H

#include <algorithm>

#include "core/util.h"
#include "core/roles.h"

namespace mempko { namespace muda { namespace context {

    template <class m>
        class modify_text_context 
        {
            public:
                modify_text_context(role::modifiable_object<m, text_type>& md, const text_type& text) : 
                    _m(md), _text(text) {}

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
                    _m(md), _list(list) {}

                void operator()()
                {
                    //find max
                    typedef typename list_type::const_iterator m_const_iterator;
                    typedef typename list_type::value_type list_value;
                    m_const_iterator max = std::max_element(
                            _list.begin(), _list.end(), util::comp_ptr_by_id<list_value>());
                    BOOST_ASSERT(*max != 0);

                    //increment
                    id_type id = max != _list.end() ? (*max)->id() + 1: 0;

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
                _m(mp), _list(l) 
            {
                BOOST_ASSERT(_m);
            }

            void operator()()
            {
                typedef typename list::list_type list_type;
                set_incremental_id<m, list_type> set_id(*_m, _list);
                role::appendable<m_ptr>* sink = &_list;
                BOOST_ASSERT(sink);

                set_id();
                _m->stamp();
                sink->add(_m);
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
                    bool removed = _removable.remove(_id);
                    BOOST_ASSERT(removed);
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

}}}//namespace


#endif

