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

#include "core/dbc.h"
#include <cstdlib>

#ifndef _WIN64
#include <execinfo.h>
#endif

namespace mempko 
{
    namespace muda 
    {
        namespace util 
        {
            namespace 
            {
                const size_t TRACE_SIZE = 16;
            }

#ifdef _WIN64
            void trace() {}
#else
            void trace()
            {
                void *t[TRACE_SIZE];
                auto size = backtrace(t, TRACE_SIZE);
                auto s = backtrace_symbols (t, size);
                for (int i = 0; i < size; i++)
                    std::cerr << s[i] << std::endl;

                std::free(s);
            }
#endif

            void raise(const char * msg) 
            {
                std::cerr << msg << std::endl;
                trace();
                exit(1);
            }

            void raise1( 
                    const char * file,
                    const char * func,
                    const int line,
                    const char * dbc,
                    const char * expr)
            {
                std::stringstream s;
                s << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                s << "!! " << dbc << " failed" << std::endl;
                s << "!! expr: " << expr << std::endl;
                s << "!! func: " << func << std::endl;
                s << "!! file: " << file << " (" << line << ")" << std::endl;
                s << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                raise(s.str().c_str());
            }
        }
    }
}
