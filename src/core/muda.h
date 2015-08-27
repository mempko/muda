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

#ifndef MEMPKO_MUDA_H
#define MEMPKO_MUDA_H

#include <list>

#include <boost/shared_ptr.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>

#include "core/types.h"
#include "core/roles.h"
#include "core/context.h"

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Auth/Dbo/AuthInfo>

namespace dbo = Wt::Dbo;
#define LOCK boost::mutex::scoped_lock lll(this->mutex())

namespace mempko 
{ 
    namespace muda 
    { 
        namespace model 
        { 
            class muda_type : 
                public role::lockable,
                public role::transitional_state<muda_type>,
                public role::ptime_stampable_when_modified<muda_type>
            {

                public:
                    muda_state state() const { return _state;} 
                    void state(muda_state s) { LOCK; _state = s; } 

                    void now() { LOCK; _state = NOW;}
                    void later() { LOCK; _state = LATER;}
                    void done() { LOCK; _state = DONE;}
                    void note() { LOCK; _state = NOTE;}

                    time modified() const { return _modified;}
                    void modified(const time& t) { LOCK; _modified = t;}

                public:
                    muda_state _state = NOW;
                    time _modified;
            };
        }
    }
}

namespace Wt {
    namespace Dbo {

         template <class Action>
             void field(Action& action, mempko::muda::model::muda_type& t,
                     const std::string& name, int size = -1)
             {
                 field(action, t._state, name + "_ts");
                 field(action, t._modified, name + "_tm");
             }
    }
}

namespace mempko 
{ 
    namespace muda 
    { 
        namespace model 
        { 

            class muda_list;
            using muda_list_dptr = dbo::ptr<muda_list>;

            class muda : 
                public role::lockable,
                public role::modifable_text<muda>,
                public role::simple_id_reciever<muda>,
                public role::ptime_stampable_when_created<muda>,
                public role::ptime_stampable_when_modified<muda>
            {
                public:
                    muda() : _text{}, _id{0}, _type{} {}

                    const text_type& text() const { return _text;}
                    void text(const text_type& text) { LOCK; _text = text;}

                    id_type id() const { return _id;}
                    void id(id_type v) { LOCK; _id = v;}

                    const muda_type& type() const {return _type;}
                    muda_type& type() {return _type;}

                    time created() const { return _created;}
                    void created(const time& t) { LOCK; _created = t;}

                    time modified() const { return _modified;}
                    void modified(const time& t) { LOCK; _modified = t;}

                private:
                    text_type _text;
                    id_type _id;
                    muda_type _type;
                    time _created;
                    time _modified;
                    muda_list_dptr _list;

                public:
                    template<class Action>
                        void persist(Action& a)
                        {
                            namespace d = Wt::Dbo;
                            dbo::field(a, _id, "mid");
                            dbo::field(a, _text, "text");
                            dbo::field(a, _created, "created");
                            dbo::field(a, _modified, "modified");
                            dbo::field(a, _type, "type");
                            dbo::belongsTo(a, _list, "list");
                        }
            };

            using muda_dptr = dbo::ptr<muda>;
            using muda_ptr_list = dbo::collection<muda_dptr>;

            class muda_list : 
                public role::lockable,
                public role::iterable_with_list<muda_list, muda_ptr_list>,
                public role::appendable_container<muda_list, muda_dptr>,
                public role::removable_container<muda_list, muda_dptr, id_type>
            {
                public:
                    const text_type& name() const { return _name;}
                    text_type& name() { return _name;}

                public:
                    using list_type = muda_ptr_list;
                    using iterator = list_type::iterator;
                    using const_iterator = list_type::const_iterator;

                    list_type& list() { return _list;}
                    const list_type& list() const { return _list;}

                    iterator begin() { return _list.begin();}
                    iterator end() { return _list.end();}
                    const_iterator begin() const { return _list.begin();}
                    const_iterator end() const { return _list.end();}

                    list_type::size_type size() const { return _list.size();}

                    template <class pred>
                    void remove_if(pred p)
                    {
                        for(auto c : _list)
                        {
                            if(p(c))
                            {
                                _list.erase(c);
                                c.remove();
                                break;
                            }
                        }
                    }

                private:
                    list_type _list;
                    text_type _name;

                public:
                    template<class Action>
                        void persist(Action& a)
                        {
                            dbo::field(a, _name, "name");
                            dbo::hasMany(a, _list, dbo::ManyToOne, "list");
                        }
            };

            using muda_list_dptr = dbo::ptr<muda_list>;

            class user
            {
                public:
                    const text_type& name() const { return _name;}
                    text_type& name() { return _name;}

                    const text_type& email() const { return _email;}
                    text_type& email() { return _email;}

                private:
                    text_type _name;
                    text_type _email;
            };

#undef LOCK
        }
    }
}

namespace mempko 
{ 
    namespace muda 
    { 
        namespace context 
        { 
            //useful typedefs
            using add_muda = add_object<model::muda,model::muda_dptr, model::muda_list>;
            using remove_muda = remove_object<model::muda_list_dptr, id_type>;
            using modify_muda_text = modify_text_context<model::muda>;

        }
    }
}

BOOST_CLASS_VERSION(mempko::muda::model::muda_type,1)
BOOST_CLASS_VERSION(mempko::muda::model::muda,1)
#endif
