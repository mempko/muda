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

#ifndef MEMPKO_DCI_ROLES_H
#define MEMPKO_DCI_ROLES_H

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "core/types.h"
#include "core/dci.h"

namespace mempko { namespace muda { namespace role {

    template< class tag>
        class signaler0
        {
            public:
                typedef boost::signals2::signal<void ()> sig;				
                typedef boost::signals2::connection connection;
                typedef typename sig::slot_type slot_type;
                connection when_signaled(const slot_type& slot) { return _sig.connect(slot);}
            protected:

                void signal() { _sig();}
            private:
                sig _sig;
        };

    template< class tag, class obj>
        class signaler1
        {
            public:
                typedef boost::signals2::signal<void (obj)> sig;				
                typedef boost::signals2::connection connection;
                typedef typename sig::slot_type slot_type;
                connection when_signaled(const slot_type& slot) { return _sig.connect(slot);}
            protected:

                void signal(obj o) { _sig(o);}

            private:
                sig _sig;
        };

    template<class m, class value>
        class modifiable_object 
        {
            public:
                virtual void change(const value&) = 0;
                virtual ~modifiable_object(){}
        };


    TAG(text_change);

    template <class m>
        class modifable_text : 
            public modifiable_object<m, text_type>,
            public signaler0<text_change> 
        {
            ADD_SELF(m)
            public:
                virtual void change(const text_type& text) 
                {
                    self()->text(text);
                    self()->stamp_modified();
                    BOOST_ASSERT(self()->text() == text);
                    signal();
                }

            public:
                connection when_text_changes(const slot_type& slot) { return when_signaled(slot);}
        };

    template<class m>
        class id_reciever
        {
            public:
                virtual void recieve(id_type id) = 0;
                virtual ~id_reciever(){}
        };

    template<class m>
        class simple_id_reciever : public id_reciever<m>
        {
            ADD_SELF(m)
            public:
                virtual void recieve(id_type id)
                {
                    self()->id(id);
                    BOOST_ASSERT(self()->id() == id);
                }
        };

    template<class list>
        class iterable
        {
            public:
                typedef typename list::iterator iterator;
                typedef typename list::const_iterator const_iterator;
                virtual iterator begin() = 0;
                virtual iterator end() = 0; 
                virtual const_iterator begin() const  = 0;
                virtual const_iterator end() const = 0; 
                virtual ~iterable(){}
        };

    template<class list, class list_type>
        class iterable_with_list : public iterable<list_type>
        {
            ADD_SELF(list)
            public:
                typedef typename list_type::iterator iterator;
                typedef typename list_type::const_iterator const_iterator;
                virtual iterator begin() { return self()->begin();}
                virtual iterator end() { return self()->end();} 
                virtual const_iterator begin() const { return self()->begin();}
                virtual const_iterator end() const { return self()->end();} 
        };

    template<class object>
        class appendable
        {
            public:
                virtual bool add(object& obj) = 0;
                virtual ~appendable(){}
        };

    TAG(when_appended)

    template<class container, class object>
        class appendable_container : 
            public appendable<object>,
            public signaler0<when_appended>
        {
            ADD_SELF(container)
            public:
                virtual bool add(object& obj) 
                {
                    self()->list().push_back(obj);
                    signal();
                    return true;
                }

            public:
                connection when_object_added(const slot_type& slot) { return when_signaled(slot);}
        };

    template<class handle>
        class removable
        {
            public:
                virtual bool remove(handle m) = 0;
                virtual ~removable(){}
        };

    template<class object_ptr, class id>
        struct id_is
        {
            public:
                id_is(id v, object_ptr& removed_obj) : 
                    _v(v), _removed_obj(removed_obj) {}
                bool operator()(object_ptr obj) 
                { 
                    bool remove = obj->id() == _v; 
                    if(remove) _removed_obj = obj;
                    return remove;
                }
            private:
                id _v;
                object_ptr& _removed_obj;
        };

    TAG(when_removed)

    template<class container, class object_ptr, class id>
        class removable_container : 
            public removable<id>,
            public signaler1<when_removed, object_ptr>
        {
            ADD_SELF(container)
            public:
                virtual bool remove(id v)
                {
                    object_ptr removed_obj;
                    id_is<object_ptr, id> pred(v, removed_obj);
                    self()->list().remove_if(pred);
                    if(removed_obj) signal(removed_obj);
                    return removed_obj;
                }

            public:
                typedef typename signaler1<when_removed, object_ptr>::connection connection;
                typedef typename signaler1<when_removed, object_ptr>::slot_type slot_type;
                connection when_object_removed(const slot_type& slot) { return when_signaled(slot);}
        };

    class transitional_object
    {
        public:
            virtual void transition() = 0;
            virtual ~transitional_object(){}
    };

    TAG(when_state_changes)

    template<class type>
    class transitional_state : 
        public transitional_object,
        public signaler0<when_state_changes>

    {
        ADD_SELF(type)
        public:
            virtual void transition()
            {
                muda_state initial_state = self()->state();
                switch(initial_state)
                {
                    case LATER: self()->now(); break;
                    case NOW: self()->done(); break;
                    case DONE: self()->note(); break;
                    case NOTE: self()->later(); break;
                }
                BOOST_ASSERT(initial_state != self()->state());
                self()->stamp_modified();
                signal();
            }
        public:
            connection when_type_changes(const slot_type& slot) { return when_signaled(slot);}
    };

    template<class type>
        class time_stampable_when_created
        {
            public:
                virtual void stamp() = 0;
        };

    template <class type>
        class ptime_stampable_when_created : public time_stampable_when_created<type>
        {
            ADD_SELF(type)
            public:
                virtual void stamp()
                {
                    using namespace boost::gregorian;
                    using namespace boost::posix_time;
                    ptime now = second_clock::local_time();
                    self()->created(now);
                }
        };

    template<class type>
        class time_stampable_when_modified
        {
            public:
                virtual void stamp_modified() = 0;
        };

    template <class type>
        class ptime_stampable_when_modified : public time_stampable_when_modified<type>
        {
            ADD_SELF(type)
            public:
                virtual void stamp_modified()
                {
                    using namespace boost::posix_time;
                    using namespace boost::gregorian;
                    time now = second_clock::local_time();
                    self()->modified(now);
                }
        };

}}} //namespace

#endif


