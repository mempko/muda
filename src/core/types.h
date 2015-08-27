#ifndef MEMPKO_MUDA_TYPES
#define MEMPKO_MUDA_TYPES

#include <string>

namespace mempko { namespace muda {

    typedef std::wstring text_type;
    typedef long id_type;
    enum muda_state { NOW, LATER, DONE, NOTE};

}}//namespace 
#endif
