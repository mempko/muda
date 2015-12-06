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
#ifndef MEMPKO_MUDA_RW_USER_H
#define MEMPKO_MUDA_RW_USER_H

#include <fstream>
#include <functional>
#include <memory>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/Auth/AuthModel>
#include <Wt/Auth/AuthWidget>
#include <Wt/WServer>
#include <Wt/WEnvironment>

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include "core/muda.h"
#include "core/context.h"
#include "wtui/mudalistwidget.h"
#include "wtui/session.h"

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            using optional_regex = boost::optional<boost::regex>;

            class rw_user : public Wt::WApplication
            {
                public:
                    rw_user(const Wt::WEnvironment& env);
                private:
                    void set_view();
                    void login_screen();
                    void settings_screen();
                    void oauth_event();
                    void startup_muda_screen();
                    void setup_view();
                    void create_header_ui();
                    void logout();
                    void triage_view();
                    void now_view();
                    void later_view();
                    void done_view();
                    void note_view();
                    Wt::WContainerWidget* create_menu();

                private:
                    bool do_search();
                    void add_new_triage_muda(muda_list_widget* mudas);
                    void add_new_now_muda(muda_list_widget* mudas);
                    void add_new_later_muda(muda_list_widget* mudas);
                    void add_new_done_muda(muda_list_widget* mudas);
                    void add_new_note_muda(muda_list_widget* mudas);
                    model::muda_dptr add_new_muda();
                    void add_muda_to_list_widget(model::muda_dptr muda, muda_list_widget*);

                private:
                    bool filter_by_search(model::muda_dptr muda);
                    bool filter_by_now(model::muda_dptr muda);
                    bool filter_by_later(model::muda_dptr muda);
                    bool filter_by_done(model::muda_dptr muda);
                    bool filter_by_note(model::muda_dptr muda);
                    void make_now_view(muda_vec&);
                    void make_later_view(muda_vec&);
                    void make_done_view(muda_vec&);
                    void make_note_view(muda_vec&);
                    void make_prioritize_view(muda_vec&);
                    void set_search();
                    void clear_search();

                private:
                    void clear_connections();
                    void save_mudas();
                    void load_user();

                private:
                    void handle_path();

                private:
                    //login widgets
                    optional_regex _search;
                    optional_regex _set_search;
                    text_type _user_name = "unknown";
                    text_type _user_email = "unknown";
                    wo::AuthWidget* _authw = nullptr;
                    connections _connections;

                private:
                    //muda widgets
                    Wt::WLineEdit* _new_muda = nullptr;
                    session_ptr _session;
                    model::user_dptr _user;
                    model::muda_list_dptr _mudas;

            };

            Wt::WApplication *create_application(const Wt::WEnvironment& env);
        }
    }
}

#endif
