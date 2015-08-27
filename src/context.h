#ifndef MEMPKO_DCI_CONTEXT_H
#define MEMPKO_DCI_CONTEXT_H

#include "roles.h"

namespace mempko { namespace muda { namespace context {

    template <class m>
        class modify_text_context 
        {
            public:
                modify_text_context(role::modifiable_text<m>& md, const text_type& text) : 
                    _m(md), _text(text) {}

                void operator()()
                {
                    _m.change(_text);
                }
            private:
                role::modifiable_text<m>& _m;
                text_type _text;
        };

}}}//namespace


#endif

