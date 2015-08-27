#ifndef MEMPKO_MUDA_H
#define MEMPKO_MUDA_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

#include "types.h"
#include "roles.h"

namespace mempko { namespace muda { namespace model{ 

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

}}}//namespace

#endif
