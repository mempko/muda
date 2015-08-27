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

#include "wtui/session.h"

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            namespace 
            {
                wo::AuthService auth_service;
                wo::PasswordService password_service{auth_service};
                oauth_service_list oauth_services;
                oauth_service_ptr_list oauth_service_ptrs;
            }

            void session::configure_auth()
            {
                auth_service.setAuthTokensEnabled(true, "muda");
                auth_service.setEmailVerificationEnabled(true);

                auto verifier = new wo::PasswordVerifier;
                verifier->addHashFunction(new wo::BCryptHashFunction{7});

                password_service.setVerifier(verifier);
                password_service.setAttemptThrottlingEnabled(true);

                if(wo::GoogleService::configured())
                    oauth_services.push_back(std::make_shared<wo::GoogleService>(auth_service));
            }

            session::session(const std::string& db)
            {
                connect(db);
            }

            void session::connect(const std::string& db)
            {
                _con.connect(db);
                _session.setConnection(_con);

                _session.mapClass<model::muda>("muda");
                _session.mapClass<model::muda_list>("mlist");
                _session.mapClass<model::user>("muser");
                _session.mapClass<model::auth_info>("auths");
                _session.mapClass<model::auth_info::AuthIdentityType>("auth_identity");
                _session.mapClass<model::auth_info::AuthTokenType>("auth_token");

                _users.reset(new user_database{_session});

                dbo::Transaction t{_session};
                try
                {
                    _session.createTables();

                    auto guest = _users->registerNew();
                    guest.addIdentity(wo::Identity::LoginName, "guest");
                    password_service.updatePassword(guest, "guest");
                    Wt::log("info") << "Database created";
                }
                catch(std::exception& e)
                {
                    Wt::log("info") << "Using existing database: " << e.what();
                }

                t.commit();
            }

            model::user_dptr session::user() const
            {
                REQUIRE(_login.loggedIn());
                INVARIANT(_users);

                auto auth = _users->find(_login.user());
                CHECK(auth);

                auto user = auth->user();
                if(!user)
                {
                    user = _session.add(new model::user);
                    auth.modify()->setUser(user);
                }
                return user;
            }

            std::string session::user_name() const
            {
                REQUIRE(_login.loggedIn());
                return _login.user().identity(wo::Identity::LoginName).toUTF8();
            }

            std::string session::email() const
            {
                REQUIRE(_login.loggedIn());
                return _login.user().email();
            }

            wo::AbstractUserDatabase& session::users()
            {
                INVARIANT(_users);
                return *_users;
            }

            const wo::AuthService& session::auth()
            {
                return auth_service;
            }

            const wo::AbstractPasswordService& session::password_auth()
            {
                return password_service;
            }

            const oauth_service_ptr_list& session::oauth()
            {
                if(oauth_service_ptrs.empty())
                    for(auto a : oauth_services)
                        oauth_service_ptrs.push_back(a.get());
                return oauth_service_ptrs;
            };
        }
    }
}
