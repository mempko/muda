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
                modify_text_context(role::modifiable_text<m>& md, const text_type& text) : 
                    _m(md), _text(text) {}

                void operator()()
                {
                    _m.change(_text);
                }
            private:
                role::modifiable_text<m>& _m;
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
            add_object(m_ptr m, list& list) : 
                _m(m), _list(list) 
            {
                BOOST_ASSERT(_m);
            }

            void operator()()
            {
                typedef typename list::list_type list_type;
                set_incremental_id<m, list_type> set_id(*_m, _list);
                role::object_sink<m_ptr>* sink = &_list;
                BOOST_ASSERT(sink);

                set_id();
                sink->add(_m);
            }

            private:
            m_ptr _m;
            list& _list;
        };

}}}//namespace


#endif

