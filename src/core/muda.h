#ifndef MEMPKO_MUDA_H
#define MEMPKO_MUDA_H

#include <list>

#include <boost/shared_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

#include "core/types.h"
#include "core/roles.h"
#include "core/context.h"

namespace mempko { namespace muda { namespace model { 

	class muda : 
        public role::modifable_text_and_notify<muda>,
        public role::simple_id_reciever<muda>
    {
        public:
            const text_type& text() const { return _text;}
            void text(const text_type& text) { _text = text;}

        private:
            text_type _text;

        private:
            template<class Archive>
                void serialize(Archive & ar, const unsigned int version)
                {
                    ar & make_nvp("text", _text);
                }
    };

    typedef boost::shared_ptr<muda> muda_ptr;
    typedef std::list<muda_ptr> muda_ptr_list;

    class muda_list : 
        public role::iterable_with_list<muda_list, muda_ptr_list>,
        public role::appendable_object_sink_and_notifier<muda_list,muda_ptr>
    {
        public:
            typedef muda_ptr_list list_type;
            typedef list_type::iterator iterator;
            typedef list_type::const_iterator const_iterator;

            list_type& list() { return _list;}
            const list_type& list() const { return _list;}

            iterator begin() { return _list.begin();}
            iterator end() { return _list.end();}
            const_iterator begin() const { return _list.begin();}
            const_iterator end() const { return _list.end();}

            list_type::size_type size() const { return _list.size();}
        private:
            list_type _list;
    };

    class text_value : public role::modifable_text_and_notify<text_value>
    {
        public:
            const text_type& text() const { return _text;}
            const void text(const text_type& t) { _text = t;}
        private:
            text_type _text;

        private:
            template<class Archive>
                void serialize(Archive & ar, const unsigned int version)
                {
                    ar & make_nvp("text", _text);
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

            template<class Archive>
                void serialize(Archive & ar, const unsigned int version)
                {
                    ar & make_nvp("name", _name);
                    ar & make_nvp("email", _email);
                }
    };




}}}//namespace

namespace mempko { namespace muda { namespace context { 
    //useful typedefs
    typedef add_object<model::muda,model::muda_ptr,model::muda_list> add_muda;

}}}//namespace

#endif
