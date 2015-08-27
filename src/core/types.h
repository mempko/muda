#ifndef MEMPKO_MUDA_TYPES
#define MEMPKO_MUDA_TYPES

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <string>

namespace mempko { namespace muda {

    typedef std::wstring text_type;
    typedef long id_type;
    enum muda_state { NOW, LATER, DONE, NOTE};

    typedef boost::gregorian::date date;
    typedef boost::posix_time::ptime time;

}}//namespace 
#endif
