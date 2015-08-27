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
        };

    template <class m>
        class modifable_text_and_notify : public modifiable_object<m, text_type> 
        {
            ADD_SELF(m)
            public:
                virtual void change(const text_type& text) 
                {
                    self()->text(text);
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
                typedef typename sig::slot_type slot_type;
                void when_object_added(const slot_type& slot) { _sig.connect(slot);}
            private:
                sig _sig;
        };

    template<class handle>
        class removable_container
        {
            public:
                virtual bool remove(handle m) = 0;
        };

    template<class container, class object_ptr, class id>
        class removable_container_and_notifier : public removable_container<id>
        {
            ADD_SELF(container)
            public:
                virtual bool remove(id v)
                {
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
                    id_is pred(v);
                    self()->list().remove_if(pred);
                    if(pred.removed_obj) _sig(pred.removed_obj);
                    return pred.removed_obj;
                }

            public:
                typedef boost::signals2::signal<void (object_ptr)> sig;				
                typedef typename sig::slot_type slot_type;
                void when_object_removed(const slot_type& slot) { _sig.connect(slot);}
            private:
                sig _sig;
        };

    class transitional_object
    {
        public:
            virtual void transition() = 0;
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
                    case NOW: self()->done(); break;
                    case LATER: self()->now(); break;
                    case DONE: self()->note(); break;
                    case NOTE: self()->now(); break;
                }
                BOOST_ASSERT(initial_state != self()->state());
                _sig();
            }
        public:
            typedef boost::signals2::signal<void ()> sig;				
            typedef typename sig::slot_type slot_type;
            void when_type_changes(const slot_type& slot) { _sig.connect(slot);}

        private:
            sig _sig;
    };

}}} //namespace

#endif


