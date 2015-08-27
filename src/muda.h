#ifndef MEMPKO_MUDA_H
#define MEMPKO_MUDA_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

#include "types.h"
#include "roles.h"

namespace mempko { namespace muda { namespace model { 

	class muda : public role::modifable_text_and_notify<muda>
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

#endif
