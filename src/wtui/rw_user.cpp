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

#include "wtui/rw_user.h"

using namespace Wt;
namespace ph = std::placeholders;

namespace mm = mempko::muda::model;
namespace mc = mempko::muda::context;
namespace mu = mempko::muda::util;

using connections = std::list<boost::signals2::connection>;

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            namespace 
            {
                void muda_list_sort(muda_vec& v)
                {
                    std::sort(std::begin(v), std::end(v), 
                            [](const auto& a, const auto& b) { return a->id() < b->id();});
                }

                WLabel* create_small_label(WString text, WString css_class)
                {
                    WLabel* l = new WLabel{text};
                    l->setStyleClass(css_class);

                    ENSURE(l);
                    return l;
                }

                WLabel* create_menu_label(WString text, WString css_class)
                {
                    WLabel* l = new WLabel{text};
                    l->setStyleClass(css_class);
                    l->resize(WLength(100, WLength::Percentage), WLength::Auto);

                    ENSURE(l);
                    return l;
                }

            }

            rw_user::rw_user(const WEnvironment& env) : WApplication{env}
            {
                std::string db;

                if(!env.server()->readConfigurationProperty("db", db))
                    throw std::invalid_argument("db configuration is missing");


                _session = std::make_shared<wt::session>(db);
                _session->login().changed().connect(this, &rw_user::oauth_event);

                internalPathChanged().connect(this, &rw_user::handle_path);
                new WAnchor(WLink(WLink::InternalPath, "/mempko"),
                        "/mempko", root());
                Wt::log("info") << "internal paht: " << internalPath();
                set_view();

                ENSURE(_session);
            }

            void rw_user::login_screen()
            {
                INVARIANT(_session);
                REQUIRE_FALSE(_session->login().loggedIn());
                INVARIANT(root());

                root()->clear();
                root()->addStyleClass("container");

                auto auth_model = new wo::AuthModel{session::auth(), _session->users(), this};
                auth_model->addPasswordAuth(&session::password_auth());
                auth_model->addOAuth(session::oauth());

                _authw = new wo::AuthWidget{_session->login()};
                _authw->setModel(auth_model);
                _authw->setRegistrationEnabled(true);
                _authw->processEnvironment();

                /////////////////////////////////////////

                root()->addWidget(new WText{"<div class='login-header'>Muda</div>"});

                if(!_authw->login().loggedIn())
                    root()->addWidget(_authw);
                else
                    startup_muda_screen();
            }

            void rw_user::settings_screen()
            {
                INVARIANT(root());
                INVARIANT(_session);
                REQUIRE(_session->login().loggedIn());
                REQUIRE(_user);

                setup_view();

                //hide new muda widget
                CHECK(_new_muda);
                _new_muda->hide();

                root()->addWidget(new WText{_user_name});

                auto auth_model = new wo::AuthModel{session::auth(), _session->users(), this};
                auth_model->addPasswordAuth(&session::password_auth());
                auth_model->addOAuth(session::oauth());

                _authw = new wo::AuthWidget{_session->login()};
                _authw->setModel(auth_model);

                auto pw = _authw->createUpdatePasswordView(_session->login().user(), true);
                root()->addWidget(pw);

                ENSURE(_user);
                ENSURE(_mudas);
            }

            void rw_user::handle_path()
            {
                auto internal_path = internalPath();
                if (_session->login().loggedIn()) {
                    Wt::log("info") << "logged in. path: " << internal_path;
                    //WApplication::instance()->setInternalPath("/play",  true);
                }
                else
                {
                    Wt::log("info") << "path: " << internal_path;
                }
            }

            void rw_user::set_view()
            {
                INVARIANT(_session);

                if(_session->login().loggedIn()) startup_muda_screen();
                else login_screen();
            }

            void rw_user::oauth_event()
            {
                INVARIANT(_session);

                set_view();
            }

            void rw_user::startup_muda_screen()
            {
                INVARIANT(root());
                INVARIANT(_session);
                REQUIRE(_session->login().loggedIn());

                root()->clear();

                load_user();

                setTitle(_user_name);
                triage_view();

                ENSURE(_user);
                ENSURE(_mudas);
            }

            void rw_user::setup_view()
            {
                INVARIANT(root());

                clear_connections();
                root()->clear();
                create_header_ui();

                _search = _set_search ? _set_search : optional_regex();
                _set_search = optional_regex();
            }

            void rw_user::triage_view()
            {
                INVARIANT(root());
                INVARIANT(_session);

                setup_view();

                //create muda list widget
                auto triage = new muda_list_widget{_session->dbs(), _mudas, 
                    std::bind(&rw_user::make_prioritize_view, this, ph::_1), root()};

                _connections.push_back(triage->when_model_updated(std::bind(&rw_user::save_mudas, this)));

                //add muda when enter is pressed
                _new_muda->enterPressed().connect(
                        std::bind(&rw_user::add_new_triage_muda, this, triage));
            }

            void rw_user::logout()
            {
                _session->logout();
            }

            void rw_user::now_view()
            {
                INVARIANT(root());
                INVARIANT(_session);

                setup_view();

                //create muda list widget
                auto list_widget = new muda_list_widget{_session->dbs(), _mudas, 
                    std::bind(&rw_user::make_now_view, this, ph::_1), root()};

                _connections.push_back(list_widget->when_model_updated(std::bind(&rw_user::save_mudas, this)));

                //add muda when enter is pressed
                _new_muda->enterPressed().connect(
                        std::bind(&rw_user::add_new_now_muda, this, list_widget));
            }

            void rw_user::later_view()
            {
                INVARIANT(root());
                INVARIANT(_session);

                setup_view();

                //create muda list widget
                auto list_widget = new muda_list_widget{_session->dbs(), _mudas, 
                    std::bind(&rw_user::make_later_view, this, ph::_1), root()};

                _connections.push_back(list_widget->when_model_updated(std::bind(&rw_user::save_mudas, this)));

                //add muda when enter is pressed
                _new_muda->enterPressed().connect(
                        std::bind(&rw_user::add_new_later_muda, this, list_widget));
            }

            void rw_user::done_view()
            {
                setup_view();
                INVARIANT(_session);

                //create muda list widget
                auto list_widget = new muda_list_widget{_session->dbs(), _mudas, 
                    std::bind(&rw_user::make_done_view, this, ph::_1), root()};

                _connections.push_back(list_widget->when_model_updated(std::bind(&rw_user::save_mudas, this)));

                //add muda when enter is pressed
                _new_muda->enterPressed().connect(
                        std::bind(&rw_user::add_new_done_muda, this, list_widget));
            }

            void rw_user::note_view()
            {
                setup_view();
                INVARIANT(_session);

                //create muda list widget
                auto list_widget = new muda_list_widget{_session->dbs(), _mudas, 
                    std::bind(&rw_user::make_note_view, this, ph::_1), root()};

                _connections.push_back(list_widget->when_model_updated(std::bind(&rw_user::save_mudas, this)));

                //add muda when enter is pressed
                _new_muda->enterPressed().connect(
                        std::bind(&rw_user::add_new_note_muda, this, list_widget));
            }

            void rw_user::clear_connections()
            {
                for(auto& c: _connections) c.disconnect();
                _connections.clear();
            }

            void rw_user::save_mudas()
            {
                //_session.flush();
            }

            void rw_user::load_user()
            {
                INVARIANT(_session);

                dbo::Transaction t{_session->dbs()};

                _user_name = _session->user_name();
                _user_email = _session->email();

                _user = _session->user();
                if(!_user)
                    throw std::runtime_error{"no user with: " + _user_name};

                if(_user->name().empty())
                    _user.modify()->name() = _user_name;

                CHECK(_user);

                auto& lists = _user.modify()->lists();
                if(lists.size() == 0)
                {
                    auto ml = new mm::muda_list;
                    ml->name() = "main";
                    auto list = _session->dbs().add(ml);
                    lists.insert(list);
                }

                _mudas = lists.front();
            }

            WContainerWidget* rw_user::create_menu()
            {
                //setup menu
                auto triage = create_menu_label("prioritize", "btn muda-pri-button");
                triage->clicked().connect (std::bind(&rw_user::triage_view, this));

                auto now = create_menu_label("now", "btn muda-now-button");
                now->clicked().connect (std::bind(&rw_user::now_view, this));

                auto later = create_menu_label("later", "btn muda-later-button");
                later->clicked().connect (std::bind(&rw_user::later_view, this));

                auto done = create_menu_label("done", "btn muda-done-button");
                done->clicked().connect (std::bind(&rw_user::done_view, this));

                auto note = create_menu_label("note", "btn muda-note-button");
                note->clicked().connect (std::bind(&rw_user::note_view, this));


                std::vector<WLabel*> menu = { triage, now, later, done, note};

                auto tabs = new WContainerWidget;
                auto layout = new WHBoxLayout;

                layout->setSpacing(0);

                for(auto m : menu)
                {
                    layout->addWidget(m);
                    layout->setStretchFactor(m, 1);
                    layout->addSpacing(WLength(8, WLength::Pixel));
                }

                //setup settings buttons
                auto settings = create_small_label("&#128100;", "sbtn muda-settings-button");
                settings->setToolTip("settings");
                settings->clicked().connect(std::bind(&rw_user::settings_screen, this));

                auto logout = create_small_label("&#59201;", "sbtn muda-settings-button");
                logout->clicked().connect (std::bind(&rw_user::logout, this));
                logout->setToolTip("logout");

                std::vector<WLabel*> small_menu = { settings, logout};
                for(auto m : small_menu)
                {
                    layout->addWidget(m);
                }

                layout->setContentsMargins(0,0,0,0);

                tabs->setLayout(layout);
                tabs->setStyleClass("menu-container");

                ENSURE(tabs);
                return tabs;
            }

            void rw_user::create_header_ui()
            {
                INVARIANT(root());

                auto menu = create_menu();

                root()->addWidget(menu);

                _new_muda = new WLineEdit;
                _new_muda->setStyleClass("new-muda");
                _new_muda->setFocus();

                auto hbox = new WHBoxLayout;
                hbox->setSpacing(0);
                hbox->addWidget(_new_muda);
                hbox->setContentsMargins(0,0,0,0);

                auto container = new WContainerWidget;
                container->setLayout(hbox);
                root()->addWidget(container);

                ENSURE(_new_muda)
            }

            void rw_user::set_search()
                try
                {
                    INVARIANT(_new_muda);
                    INVARIANT_GREATER(_new_muda->text().value().size(), 0);

                    std::string anyChar{".*"};
                    std::string search = _new_muda->text().toUTF8().substr(1);
                    std::string regex = anyChar + search + anyChar;

                    boost::regex e{regex};
                    _set_search = boost::make_optional(e);
                }
            catch(std::exception& e)
            {
                std::cerr << "regex error" << e.what() << std::endl;
            }

            void rw_user::clear_search()
            {
                _search.reset();
                _set_search.reset();
            }

            bool rw_user::filter_by_search(mm::muda_dptr muda)
                try
                {
                    REQUIRE(muda);

                    if(!_search) return true;
                    return boost::regex_match(muda->text(), _search.get());
                }
            catch(...)
            {
                return false;
            }

            bool rw_user::filter_by_now(mm::muda_dptr muda)
            {
                REQUIRE(muda);
                return muda->type().state() == NOW && filter_by_search(muda);
            }

            bool rw_user::filter_by_later(mm::muda_dptr muda)
            {
                REQUIRE(muda);
                return muda->type().state() == LATER && filter_by_search(muda);
            }

            bool rw_user::filter_by_done(mm::muda_dptr muda)
            {
                REQUIRE(muda);
                return muda->type().state() == DONE && filter_by_search(muda);
            }

            bool rw_user::filter_by_note(mm::muda_dptr muda)
            {
                REQUIRE(muda);
                return muda->type().state() == NOTE && filter_by_search(muda);
            }


            void rw_user::make_now_view(muda_vec& v)
            {
                mu::filter(v, [&](auto muda) { return this->filter_by_now(muda);});
                muda_list_sort(v);
            }

            void rw_user::make_later_view(muda_vec& v)
            {
                mu::filter(v, [&](auto muda) { return this->filter_by_later(muda);});
                muda_list_sort(v);
            }

            void rw_user::make_done_view(muda_vec& v)
            {
                mu::filter(v, [&](auto muda) { return this->filter_by_done(muda);});
                muda_list_sort(v);
            }

            void rw_user::make_note_view(muda_vec& v)
            {
                mu::filter(v, [&](auto muda) { return this->filter_by_note(muda);});
                muda_list_sort(v);
            }

            void rw_user::make_prioritize_view(muda_vec& v)
            {
                //filter
                mu::filter(v,
                        [&](auto muda) 
                        { 
                        return muda->type().state() != NOTE && this->filter_by_search(muda);
                        }); 

                muda_list_sort(v);

                //put done first, then later, then now
                //This order is important because it will show up in reverse when added to the muda list
                std::stable_sort(
                        v.begin(), v.end(), [](auto a, auto b)
                        { 
                        return static_cast<int>(a->type().state()) > static_cast<int>(b->type().state());
                        }
                        );
            }

            mm::muda_dptr rw_user::add_new_muda()
            {
                INVARIANT(_new_muda);
                INVARIANT(_session);

                //create new muda and add it to muda list
                auto muda = _session->dbs().add(new mm::muda);

                mc::add_muda add{muda, *(_mudas.modify())};
                add();

                mc::modify_muda_text modify_text{*(muda.modify()), _new_muda->text().toUTF8()};
                modify_text();

                ENSURE(muda);
                return muda;
            }

            bool rw_user::do_search()
            {
                INVARIANT(_new_muda);

                auto mt = _new_muda->text().value();
                if(!mt.empty() && mt[0] == L'/') 
                {
                    set_search();
                    return true;
                }
                clear_search();
                return mt.size() == 0;
            }

            void rw_user::add_new_triage_muda(muda_list_widget* mudas)
            {
                REQUIRE(mudas);
                INVARIANT(_new_muda);
                INVARIANT(_session);

                if(do_search()) {triage_view();return;}

                dbo::Transaction t{_session->dbs()};

                auto muda = add_new_muda();
                muda.modify()->type().now();
                add_muda_to_list_widget(muda, mudas);
            }

            void rw_user::add_new_now_muda(muda_list_widget* mudas)
            {
                REQUIRE(mudas);
                INVARIANT(_new_muda);
                INVARIANT(_session);

                if(do_search()) {now_view();return;}

                dbo::Transaction t{_session->dbs()};

                auto muda = add_new_muda();
                muda.modify()->type().now();
                add_muda_to_list_widget(muda, mudas);
            }

            void rw_user::add_new_later_muda(muda_list_widget* mudas)
            {
                REQUIRE(mudas);
                INVARIANT(_new_muda);
                INVARIANT(_session);

                if(do_search()) {later_view();return;}

                dbo::Transaction t{_session->dbs()};

                auto muda = add_new_muda();
                muda.modify()->type().later();
                add_muda_to_list_widget(muda, mudas);
            }

            void rw_user::add_new_done_muda(muda_list_widget* mudas)
            {
                REQUIRE(mudas);
                INVARIANT(_new_muda);
                INVARIANT(_session);

                if(do_search()) {done_view();return;}

                dbo::Transaction t{_session->dbs()};

                auto muda = add_new_muda();
                muda.modify()->type().done();
                add_muda_to_list_widget(muda, mudas);
            }

            void rw_user::add_new_note_muda(muda_list_widget* mudas)
            {
                REQUIRE(mudas);
                INVARIANT(_new_muda);
                INVARIANT(_session);

                if(do_search()) {note_view();return;}

                dbo::Transaction t{_session->dbs()};

                auto muda = add_new_muda();
                muda.modify()->type().note();
                add_muda_to_list_widget(muda, mudas);
            }

            void rw_user::add_muda_to_list_widget(mm::muda_dptr muda, muda_list_widget* list_widget)
            {
                REQUIRE(muda);
                REQUIRE(list_widget);
                INVARIANT(_new_muda);

                list_widget->add_muda(muda);
                _new_muda->setText(L"");

                save_mudas();
            }

            WApplication *create_application(const WEnvironment& env)
            {
                auto a = new rw_user{env};

                a->useStyleSheet("resources/style.css");
                a->useStyleSheet("resources/fonts/entypo.css");
                a->setTitle("Muda");                               
                a->messageResourceBundle().use(a->appRoot() + "/templates");

                return a;
            }
        }
    }
}
