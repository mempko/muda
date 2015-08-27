#ifndef MEMPKO_DCI_ROLES_H
#define MEMPKO_DCI_ROLES_H

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "core/types.h"
#include "core/dci.h"

namespace mempko { namespace muda { namespace role {

    template<class m, class value>
        class modifiable_object 
        {
            public:
                virtual void change(const value&) = 0;
                virtual ~modifiable_object(){}
        };

    template <class m>
        class modifable_text_and_notify : public modifiable_object<m, text_type> 
        {
            ADD_SELF(m)
            public:
                virtual void change(const text_type& text) 
                {
                    self()->text(text);
                    self()->stamp_modified();
                    BOOST_ASSERT(self()->text() == text);
                    _sig();
                }

            public:
                typedef boost::signals2::signal<void ()> sig;				
                typedef boost::signals2::connection connection;
                typedef typename sig::slot_type slot_type;
                connection when_text_changes(const slot_type& slot) { return _sig.connect(slot);}
            private:
                sig _sig;

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
        class appendable_container
        {
            public:
                virtual bool add(object& obj) = 0;
                virtual ~appendable_container(){}
        };

    template<class container, class object>
        class appendable_container_and_notifier : public appendable_container<object>
        {
            ADD_SELF(container)
            public:
                virtual bool add(object& obj) 
                {
                    self()->list().push_back(obj);
                    _sig();
                    return true;
                }
            public:
                typedef boost::signals2::signal<void ()> sig;				
                typedef boost::signals2::connection connection;
                typedef typename sig::slot_type slot_type;
                connection when_object_added(const slot_type& slot) { return _sig.connect(slot);}
            private:
                sig _sig;
        };

    template<class handle>
        class removable_container
        {
            public:
                virtual bool remove(handle m) = 0;
                virtual ~removable_container(){}
        };

    template<class object_ptr, class id>
        struct id_is
        {
            id_is(id v) : _v(v) {}
            bool operator()(object_ptr obj) 
            { 
                bool remove = obj->id() == _v; 
                if(remove) removed_obj = obj;
                return remove;
            }
            id _v;
            object_ptr removed_obj;
        };

    template<class container, class object_ptr, class id>
        class removable_container_and_notifier : public removable_container<id>
        {
            ADD_SELF(container)
            public:
                virtual bool remove(id v)
                {
                    id_is<object_ptr, id> pred(v);
                    self()->list().remove_if(pred);
                    if(pred.removed_obj) _sig(pred.removed_obj);
                    return pred.removed_obj;
                }

            public:
                typedef boost::signals2::signal<void (object_ptr)> sig;				
                typedef boost::signals2::connection connection;
                typedef typename sig::slot_type slot_type;
                connection when_object_removed(const slot_type& slot) { return _sig.connect(slot);}
            private:
                sig _sig;
        };

    class transitional_object
    {
        public:
            virtual void transition() = 0;
            virtual ~transitional_object(){}
    };

    template<class type>
    class transitional_state_and_notify : public transitional_object
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
                _sig();
            }
        public:
            typedef boost::signals2::signal<void ()> sig;				
            typedef boost::signals2::connection connection;
            typedef typename sig::slot_type slot_type;
            connection when_type_changes(const slot_type& slot) { return _sig.connect(slot);}

        private:
            sig _sig;
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


