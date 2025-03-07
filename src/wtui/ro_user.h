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
#ifndef MEMPKO_MUDA_RO_USER_H
#define MEMPKO_MUDA_RO_USER_H

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <regex>

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/Auth/AuthModel.h>
#include <Wt/Auth/AuthWidget.h>
#include <Wt/WServer.h>
#include <Wt/WEnvironment.h>

#include "core/muda.h"
#include "wtui/romudalistwidget.h"
#include "wtui/session.h"



namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            using optional_regex = std::optional<std::regex>;
            extern const std::string RO_APP_PATH;

            class ro_user : public Wt::WApplication
            {
                public:
                    ro_user(const Wt::WEnvironment& env);

                private:
                    void set_view();
                    void oauth_event();
                    void startup_muda_screen();
                    void setup_view();
                    void create_header_ui();
                    void triage_view();
                    void now_view();
                    void later_view();
                    void done_view();
                    void note_view();
                    std::unique_ptr<Wt::WContainerWidget> create_menu();

                private:
                    bool do_search();

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
                    void load_user();

                private:
                    void handle_path();

                private:
                    //login widgets
                    optional_regex _search;
                    optional_regex _set_search;
                    text_type _user_name;

                private:
                    //muda widgets
                    Wt::WLineEdit* _search_box = nullptr;
                    session_ptr _session;
                    model::user_dptr _user;
                    model::muda_list_dptr _mudas;

            };

            std::unique_ptr<Wt::WApplication> create_ro_user_application(const Wt::WEnvironment& env);
        }
    }
}
#endif
