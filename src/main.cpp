/*
 * Copyright (C) 2012  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

using namespace Wt;
namespace ph = std::placeholders;

namespace m = mempko::muda;
namespace mm = mempko::muda::model;
namespace mc = mempko::muda::context;
namespace mt = mempko::muda::wt;

using connections = std::list<boost::signals2::connection>;
using optional_regex = boost::optional<boost::regex>;

class app : public WApplication
{
    public:
        app(const WEnvironment& env);
    private:
        void set_view();
        void login_screen();
        void oauth_event();
        void startup_muda_screen();
        void setup_view();
        void create_header_ui();
        void all_view();
        void triage_view();
        void now_view();
        void later_view();
        void done_view();
        void note_view();
        WContainerWidget* create_menu();

    private:
        bool do_search();
        void add_new_all_muda(mt::muda_list_widget* mudas);
        void add_new_triage_muda(mt::muda_list_widget* mudas);
        void add_new_now_muda(mt::muda_list_widget* mudas);
        void add_new_later_muda(mt::muda_list_widget* mudas);
        void add_new_done_muda(mt::muda_list_widget* mudas);
        void add_new_note_muda(mt::muda_list_widget* mudas);
        mm::muda_dptr add_new_muda();
        void add_muda_to_list_widget(mm::muda_dptr muda, mt::muda_list_widget*);

    private:
        bool filter_by_search(mm::muda_dptr muda);
        bool filter_by_all(mm::muda_dptr muda);
        bool filter_by_now(mm::muda_dptr muda);
        bool filter_by_later(mm::muda_dptr muda);
        bool filter_by_done(mm::muda_dptr muda);
        bool filter_by_note(mm::muda_dptr muda);
        void set_search();
        void clear_search();

    private:
        void clear_connections();
        void save_mudas();
        void load_user();

    private:
        //login widgets
        optional_regex _search;
        optional_regex _set_search;
        m::text_type _user_name = "unknown";
        m::text_type _user_email = "unknown";
        wo::AuthWidget* _authw = nullptr;
        connections _connections;

    private:
        //muda widgets
        WLineEdit* _new_muda = nullptr;
        mt::session_ptr _session;
        mm::user_dptr _user;
        mm::muda_list_dptr _mudas;

};

app::app(const WEnvironment& env) : WApplication{env}
{
    std::string db;

    if(!env.server()->readConfigurationProperty("db", db))
        throw std::invalid_argument("db configuration is missing");

    _session = std::make_shared<mt::session>(db);
    _session->login().changed().connect(this, &app::oauth_event);

    set_view();

    ENSURE(_session);
}


void app::login_screen()
{
    REQUIRE_FALSE(_session->login().loggedIn());
    INVARIANT(root());
    INVARIANT(_session);

    root()->addStyleClass("container");

    auto auth_model = new wo::AuthModel{mt::session::auth(), _session->users(), this};
    auth_model->addPasswordAuth(&mt::session::password_auth());
    auth_model->addOAuth(mt::session::oauth());

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

void app::set_view()
{
    root()->clear();

    if(_session->login().loggedIn()) startup_muda_screen();
    else login_screen();
}

void app::oauth_event()
{
    INVARIANT(_session);

    set_view();
}

void app::startup_muda_screen()
{
    INVARIANT(_session);
    _user_name = _session->user_name();
    _user_email = _session->email();

    setTitle(_user_name);
    load_user();
    all_view();
}

void app::setup_view()
{
    INVARIANT(root());

    clear_connections();
    root()->clear();
    create_header_ui();

    _search = _set_search ? _set_search : optional_regex();
    _set_search = optional_regex();
}

void app::all_view()
{
    INVARIANT(_session);
    setup_view();

    //create muda list widget
    auto list_widget = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_all, this, ph::_1), root()};

    _connections.push_back(list_widget->when_model_updated(std::bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect(
            std::bind(&app::add_new_all_muda, this, list_widget));
}

void app::triage_view()
{
    INVARIANT(root());
    INVARIANT(_session);

    setup_view();

    //create muda list widget
    auto now = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_now, this, ph::_1), root()};
    auto later = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_later, this, ph::_1), root()};
    auto done = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_done, this, ph::_1), root()};

    _connections.push_back(now->when_model_updated(std::bind(&app::save_mudas, this)));
    _connections.push_back(later->when_model_updated(std::bind(&app::save_mudas, this)));
    _connections.push_back(done->when_model_updated(std::bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect(
            std::bind(&app::add_new_triage_muda, this, now));
}

void app::now_view()
{
    INVARIANT(root());
    INVARIANT(_session);

    setup_view();

    //create muda list widget
    auto list_widget = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_now, this, ph::_1), root()};

    _connections.push_back(list_widget->when_model_updated(std::bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect(
            std::bind(&app::add_new_now_muda, this, list_widget));
}

void app::later_view()
{
    INVARIANT(root());
    INVARIANT(_session);

    setup_view();

    //create muda list widget
    auto list_widget = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_later, this, ph::_1), root()};

    _connections.push_back(list_widget->when_model_updated(std::bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect(
            std::bind(&app::add_new_later_muda, this, list_widget));
}

void app::done_view()
{
    setup_view();
    INVARIANT(_session);

    //create muda list widget
    auto list_widget = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_done, this, ph::_1), root()};

    _connections.push_back(list_widget->when_model_updated(std::bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect(
            std::bind(&app::add_new_done_muda, this, list_widget));
}

void app::note_view()
{
    setup_view();
    INVARIANT(_session);

    //create muda list widget
    auto list_widget = new mt::muda_list_widget{_session->dbs(), _mudas, 
            std::bind(&app::filter_by_note, this, ph::_1), root()};

    _connections.push_back(list_widget->when_model_updated(std::bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect(
            std::bind(&app::add_new_note_muda, this, list_widget));
}

void app::clear_connections()
{
    for(auto& c: _connections) c.disconnect();
    _connections.clear();
}

void app::save_mudas()
{
    //_session.flush();
}

void app::load_user()
{
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

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

WLabel* create_menu_label(WString text, WString css_class)
{
    WLabel* l = new WLabel{text};
    l->setStyleClass(css_class);
    l->resize(WLength(100, WLength::Percentage), WLength::Auto);

    ENSURE(l);
    return l;
}

WContainerWidget* app::create_menu()
{
    auto all = create_menu_label("all", "btn muda-all-button");
    all->clicked().connect (std::bind(&app::all_view, this));

    auto triage = create_menu_label("triage", "btn muda-all-button");
    triage->clicked().connect (std::bind(&app::triage_view, this));

    auto now = create_menu_label("now", "btn muda-now-button");
    now->clicked().connect (std::bind(&app::now_view, this));

    auto later = create_menu_label("later", "btn muda-later-button");
    later->clicked().connect (std::bind(&app::later_view, this));

    auto done = create_menu_label("done", "btn muda-done-button");
    done->clicked().connect (std::bind(&app::done_view, this));

    auto note = create_menu_label("note", "btn muda-note-button");
    note->clicked().connect (std::bind(&app::note_view, this));

    std::vector<WLabel*> menu = { all, triage, now, later, done, note};
    unsigned int last = menu.size() - 1;

    auto tabs = new WContainerWidget;
    auto layout = new WHBoxLayout;

    layout->setSpacing(0);

    for(auto m : menu)
    {
        layout->addWidget(m);
        layout->setStretchFactor(m, 1);
        if(m != note) layout->addSpacing(WLength(8, WLength::Pixel));
    }

    layout->setContentsMargins(0,0,0,0);

    tabs->setLayout(layout);
    tabs->setStyleClass("menu-container");

    ENSURE(tabs);
    return tabs;
}

void app::create_header_ui()
{
    INVARIANT(root());

    auto menu = create_menu();

    root()->addWidget(menu);

    _new_muda = new WLineEdit;
    _new_muda->resize(WLength(100, WLength::Percentage), WLength::Auto);
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

void app::set_search()
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

void app::clear_search()
{
    _search.reset();
    _set_search.reset();
}

bool app::filter_by_search(mm::muda_dptr muda)
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

bool app::filter_by_all(mm::muda_dptr muda)
{
    REQUIRE(muda);
    return filter_by_search(muda);
}

bool app::filter_by_now(mm::muda_dptr muda)
{
    REQUIRE(muda);
    return muda->type().state() == m::NOW && filter_by_search(muda);
}

bool app::filter_by_later(mm::muda_dptr muda)
{
    REQUIRE(muda);
    return muda->type().state() == m::LATER && filter_by_search(muda);
}

bool app::filter_by_done(mm::muda_dptr muda)
{
    REQUIRE(muda);
    return muda->type().state() == m::DONE && filter_by_search(muda);
}

bool app::filter_by_note(mm::muda_dptr muda)
{
    REQUIRE(muda);
    return muda->type().state() == m::NOTE && filter_by_search(muda);
}

mm::muda_dptr app::add_new_muda()
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

bool app::do_search()
{
    INVARIANT(_new_muda);

    if(_new_muda->text().value()[0] == L'/') 
    {
        set_search();
        return true;
    }
    clear_search();
    return _new_muda->text().value().size() == 0;
}

void app::add_new_all_muda(mt::muda_list_widget* mudas)
{
    REQUIRE(mudas);
    INVARIANT(_new_muda);
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

    if(do_search()) {all_view();return;}

    auto muda = add_new_muda();
    muda.modify()->type().now();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_triage_muda(mt::muda_list_widget* mudas)
{
    REQUIRE(mudas);
    INVARIANT(_new_muda);
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

    if(do_search()) {triage_view();return;}

    auto muda = add_new_muda();
    muda.modify()->type().now();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_now_muda(mt::muda_list_widget* mudas)
{
    REQUIRE(mudas);
    INVARIANT(_new_muda);
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

    if(do_search()) {now_view();return;}

    auto muda = add_new_muda();
    muda.modify()->type().now();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_later_muda(mt::muda_list_widget* mudas)
{
    REQUIRE(mudas);
    INVARIANT(_new_muda);
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

    if(do_search()) {later_view();return;}

    auto muda = add_new_muda();
    muda.modify()->type().later();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_done_muda(mt::muda_list_widget* mudas)
{
    REQUIRE(mudas);
    INVARIANT(_new_muda);
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

    if(do_search()) {done_view();return;}

    auto muda = add_new_muda();
    muda.modify()->type().done();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_note_muda(mt::muda_list_widget* mudas)
{
    REQUIRE(mudas);
    INVARIANT(_new_muda);
    INVARIANT(_session);

    dbo::Transaction t{_session->dbs()};

    if(do_search()) {note_view();return;}

    auto muda = add_new_muda();
    muda.modify()->type().note();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_muda_to_list_widget(mm::muda_dptr muda, mt::muda_list_widget* list_widget)
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
    auto a = new app{env};

    a->useStyleSheet("resources/style.css");
    a->setTitle("Muda");                               
    a->messageResourceBundle().use(a->appRoot() + "templates");

    return a;
}

int main(int argc, char **argv)
try 
{
    Wt::WServer server{argv[0]};

    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    server.addEntryPoint(Wt::Application, create_application);

    mt::session::configure_auth();

    if (server.start()) 
    {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
    return 0;
} 
catch (Wt::WServer::Exception& e) 
{
    std::cerr << e.what() << std::endl;
    return 1;
} 
catch (std::exception &e) 
{
    std::cerr << "exception: " << e.what() << std::endl;
    return 1;
}

