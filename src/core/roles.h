#ifndef MEMPKO_DCI_ROLES_H
#define MEMPKO_DCI_ROLES_H

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "core/types.h"
#include "core/dci.h"

namespace mempko { namespace muda { namespace role {

    template<class m>
        class modifiable_text 
        {
            public:
                virtual void change(const text_type&) = 0;
        };

    template <class m>
        class modifable_text_and_notify : public modifiable_text<m> 
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

    template<class t>
        class object_sink
        {
            public:
                virtual bool add(t& m) = 0;
        };

    template<class list_t, class t>
        class appendable_object_sink_and_notifier : public object_sink<t>
        {
            ADD_SELF(list_t)
            public:
                virtual bool add(t& object) 
                {
                    self()->list().push_back(object);
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

}}} //namespace

#endif


