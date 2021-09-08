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

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/Auth/Dbo/AuthInfo.h>


namespace dbo = Wt::Dbo;
#define LOCK boost::mutex::scoped_lock lll(this->mutex())

namespace mempko::muda::model
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

namespace Wt::Dbo {
    template <class Action>
        void field(Action& action, mempko::muda::model::muda_type& t,
                const std::string& name, int size = -1)
        {
            field(action, t._state, name + "_ts");
            field(action, t._modified, name + "_tm");
        }
}

namespace mempko::muda::model
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
            id_type _id = 0;
            muda_type _type;
            time _created;
            time _modified;
            muda_list_dptr _list;

        public:
            template<class Action>
                void persist(Action& a)
                {
                    namespace dbo = Wt::Dbo;
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

    class user;
    using user_dptr = dbo::ptr<user>;

    class muda_list : 
        public role::lockable,
        public role::iterable_with_list<muda_list, muda_ptr_list>,
        public role::appendable_container<muda_list, muda_dptr>,
        public role::removable_container<muda_list, muda_dptr, id_type>
    {
        public:
            const text_type& name() const { return _name;}
            text_type& name() { return _name;}

            bool is_public() const { return _is_public;}
            void is_public(bool v) { LOCK; _is_public = v;}

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

            user_dptr user() const;

        private:
            list_type _list;
            text_type _name;
            user_dptr _user;
            bool _is_public = false;

        public:
            template<class Action>
                void persist(Action& a)
                {
                    dbo::field(a, _name, "name");
                    dbo::field(a, _is_public, "is_public");
                    dbo::hasMany(a, _list, dbo::ManyToOne, "list");
                    dbo::belongsTo(a, _user, "muser");
                }
    };

    using muda_list_dptr = dbo::ptr<muda_list>;
    using muda_list_dptrs = dbo::collection<muda_list_dptr>;

    using auth_info = Wt::Auth::Dbo::AuthInfo<user>;
    using auth_info_dptr = dbo::ptr<auth_info>;
    using auth_infos = dbo::collection<auth_info_dptr>;
    using users = dbo::collection<user_dptr>;

    class user : public dbo::Dbo<user>
    {
        public:
            const text_type& name() const { return _name;}
            text_type& name() { return _name;}

            const muda_list_dptrs& lists() const { return _lists;}
            muda_list_dptrs& lists() { return _lists;}

            const auth_infos& auths() const { return _auth_infos;}
            auth_infos& auths() { return _auth_infos;}

        private:
            text_type _name;
            text_type _email;
            muda_list_dptrs _lists;
            auth_infos _auth_infos;

        public:
            template<class Action>
                void persist(Action& a)
                {
                    dbo::field(a, _name, "name");
                    dbo::hasMany(a, _lists, dbo::ManyToOne, "muser");
                    dbo::hasMany(a, _auth_infos, Wt::Dbo::ManyToOne, "user");
                }
    };

    using user_dptr = dbo::ptr<user>;

#undef LOCK
}

namespace mempko::muda::context
{ 
    //useful typedefs
    using add_muda = add_object<model::muda,model::muda_dptr, model::muda_list>;
    using remove_muda = remove_object<model::muda_list_dptr, id_type>;
    using modify_muda_text = modify_text_context<model::muda>;
}

BOOST_CLASS_VERSION(mempko::muda::model::muda_type,1)
BOOST_CLASS_VERSION(mempko::muda::model::muda,1)
#endif
