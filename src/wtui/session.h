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

#ifndef MEMPKO_MUDA_SESSION_H
#define MEMPKO_MUDA_SESSION_H

#include "core/muda.h"

#include <Wt/Dbo/backend/Postgres>
#include <Wt/Auth/Dbo/UserDatabase>

#include <Wt/Auth/Login>
#include <Wt/Auth/AuthService>
#include <Wt/Auth/HashFunction>
#include <Wt/Auth/PasswordService>
#include <Wt/Auth/PasswordStrengthValidator>
#include <Wt/Auth/PasswordVerifier>
#include <Wt/Auth/GoogleService>
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/Auth/Dbo/UserDatabase>

#include <memory>
#include <string>

namespace wo = Wt::Auth;

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            using oath_service_ptr = std::shared_ptr<wo::OAuthService>;
            using oauth_service_list = std::vector<oath_service_ptr>;
            using oauth_service_ptr_list = std::vector<const wo::OAuthService*>;
            using user_database = wo::Dbo::UserDatabase<model::auth_info>;
            using user_database_ptr = std::shared_ptr<user_database>;

            class session
            {
                public:
                    static void configure_auth();

                    session(const std::string& db);

                    const dbo::Session& dbs() const { return _session;}
                    dbo::Session& dbs() { return _session;}

                    wo::AbstractUserDatabase& users();

                    const wo::Login& login() const { return _login; }
                    wo::Login& login() { return _login; }

                    //current user
                    std::string user_name() const;
                    std::string email() const;

                    static const Wt::Auth::AuthService& auth();
                    static const Wt::Auth::AbstractPasswordService& password_auth();
                    static const oauth_service_ptr_list& oauth();

                    model::user_dptr user() const;

                private:
                    void connect(const std::string& db);

                private:
                    user_database_ptr _users;
                    wo::Login _login;

                    dbo::backend::Postgres _con;
                    mutable dbo::Session _session;
            };

            using session_ptr = std::shared_ptr<session>;
        }
    }
}

#endif
