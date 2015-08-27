#ifndef MEMPKO_DCI_ROLES_H
#define MEMPKO_DCI_ROLES_H

#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "types.h"
#include "dci.h"

namespace mempko { namespace muda { namespace role {

    template<class m>
        class modifiable_text 
        {
            public:
                virtual void change(const text_type&) = 0;
        };

    template <class m>
        class modifable_text_and_notify : public modifiable_text<m>, public dci::methodful_role<m>
        {
            public:
                virtual void change(const text_type& text) 
                {
                    self()->text(text);
                    _sig(*self());
                }

            public:
                typedef boost::signals2::signal<void (const m&)> sig;				
                typedef typename sig::slot_type slot_type;
                void when_changed(const slot_type& slot) { _sig.connect(slot);}
            private:
                sig _sig;

        };

}}} //namespace

#endif


