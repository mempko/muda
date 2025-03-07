/**
* Copyright (C) 2025  Maxim Noah Khailo
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

#include <Wt/WGlobal.h>
#include <fstream>
#include <functional>
#include <memory>

#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WEnvironment.h>

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include "wtui/rw_user.h"
#include "wtui/ro_user.h"

using namespace Wt;

namespace mt = mempko::muda::wt;

int main(int argc, char **argv) try {
    Wt::WServer server{argv[0]};

    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    server.addEntryPoint(Wt::EntryPointType::Application, mt::create_application);
    server.addEntryPoint(Wt::EntryPointType::Application, mt::create_ro_user_application, mt::RO_APP_PATH);

    mt::session::configure_auth();

    if (server.start()) {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
    return 0;
} catch (Wt::WServer::Exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
} catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 1;
}

