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

namespace mempko 
{ 
    namespace muda 
    { 
        namespace model 
        { 

#define LOCK boost::mutex::scoped_lock lll(this->mutex())

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

                private:
                    muda_state _state = NOW;
                    time _modified;

                private:
                    friend class boost::serialization::access;
                    template<class Archive>
                        void serialize(Archive & ar, const unsigned int version)
                        {
                            LOCK;
                            using namespace boost::serialization;
                            ar & make_nvp("state", _state);
                            ar & make_nvp("modified", _modified);
                        }
            };

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

                private:
                    friend class boost::serialization::access;
                    template<class Archive>
                        void serialize(Archive & ar, const unsigned int version)
                        {
                            LOCK;
                            using namespace boost::serialization;
                            ar & make_nvp("id", _id);
                            ar & make_nvp("text", _text);
                            ar & make_nvp("type", _type);
                            ar & make_nvp("created", _created);
                            ar & make_nvp("modified", _modified);
                        }
            };

            typedef boost::shared_ptr<muda> muda_ptr;
            typedef std::list<muda_ptr> muda_ptr_list;

            class muda_list : 
                public role::lockable,
                public role::iterable_with_list<muda_list, muda_ptr_list>,
                public role::appendable_container<muda_list,muda_ptr>,
                public role::removable_container<muda_list, muda_ptr, id_type>
            {
                public:
                    typedef muda_ptr_list list_type;
                    typedef list_type::iterator iterator;
                    typedef list_type::const_iterator const_iterator;
                    typedef list_type::reverse_iterator reverse_iterator;
                    typedef list_type::const_reverse_iterator const_reverse_iterator;

                    list_type& list() { return _list;}
                    const list_type& list() const { return _list;}

                    iterator begin() { return _list.begin();}
                    iterator end() { return _list.end();}
                    const_iterator begin() const { return _list.begin();}
                    const_iterator end() const { return _list.end();}
                    reverse_iterator rbegin() { return _list.rbegin();}
                    reverse_iterator rend() { return _list.rend();}
                    const_reverse_iterator rbegin() const { return _list.rbegin();}
                    const_reverse_iterator rend() const { return _list.rend();}

                    list_type::size_type size() const { return _list.size();}

                private:
                    list_type _list;

                private:
                    friend class boost::serialization::access;
                    template<class Archive>
                        void serialize(Archive & ar, const unsigned int version)
                        {
                            LOCK;
                            using namespace boost::serialization;
                            ar & make_nvp("mudas", _list);
                        }
            };

            class text_value : 
                public role::lockable,
                public role::ptime_stampable_when_modified<text_value>,
                public role::modifable_text<text_value>
            {
                public:
                    const text_type& text() const { return _text;}
                    const void text(const text_type& t) { _text = t;}

                    time modified() const { return _modified;}
                    void modified(const time& t) { _modified = t;}

                private:
                    text_type _text;
                    time _modified;

                private:
                    friend class boost::serialization::access;
                    template<class Archive>
                        void serialize(Archive & ar, const unsigned int version)
                        {
                            using namespace boost::serialization;
                            ar & make_nvp("text", _text);
                            ar & make_nvp("modified", _modified);
                        }
            };

            class user
            {
                public:
                    const text_value& name() const { return _name;}
                    text_value& name() { return _name;}

                    const text_value& email() const { return _email;}
                    text_value& email() { return _email;}

                private:
                    text_value _name;
                    text_value _email;

                private:
                    friend class boost::serialization::access;
                    template<class Archive>
                        void serialize(Archive & ar, const unsigned int version)
                        {
                            using namespace boost::serialization;
                            ar & make_nvp("name", _name);
                            ar & make_nvp("email", _email);
                        }
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
            typedef add_object<model::muda,model::muda_ptr,model::muda_list> add_muda;
            typedef remove_object<model::muda_list, id_type> remove_muda;
            typedef modify_text_context<model::muda> modify_muda_text;

        }
    }
}

BOOST_CLASS_VERSION(mempko::muda::model::muda_type,1)
BOOST_CLASS_VERSION(mempko::muda::model::muda,1)
#endif
