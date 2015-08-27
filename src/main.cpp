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

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include "core/muda.h"
#include "core/context.h"
#include "wtui/mudalistwidget.h"

using namespace Wt;
using boost::bind;
using boost::mem_fn;

namespace m = mempko::muda;
namespace mm = mempko::muda::model;
namespace mc = mempko::muda::context;
namespace mt = mempko::muda::wt;

typedef std::list<boost::signals2::connection> connections;

typedef boost::optional<boost::wregex> optional_regex;

class app : public WApplication
{
    public:
        app(const WEnvironment& env);
    private:
        void login_screen();
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
        mm::muda_ptr add_new_muda();
        void add_muda_to_list_widget(mm::muda_ptr muda, mt::muda_list_widget*);
    private:
        bool filter_by_search(mm::muda_ptr muda);
        bool filter_by_all(mm::muda_ptr muda);
        bool filter_by_now(mm::muda_ptr muda);
        bool filter_by_later(mm::muda_ptr muda);
        bool filter_by_done(mm::muda_ptr muda);
        bool filter_by_note(mm::muda_ptr muda);
        void set_search();
        void clear_search();
    private:
        void clear_connections();
        void save_mudas();
        void load_mudas();
    private:
        //login widgets
        WLineEdit* _muda_file_edit;
        optional_regex _search;
        optional_regex _set_search;
        std::string _muda_file;
        connections _connections;
    private:
        //muda widgets
        WLineEdit* _new_muda;
        mm::muda_list _mudas;
};

app::app(const WEnvironment& env) : 
    WApplication(env),
    _new_muda(0),
    _muda_file("global")
{
    login_screen();
}

void app::login_screen()
{
    root()->clear();
    WHBoxLayout* layout = new WHBoxLayout;

    layout->addWidget(new WLabel("Muda List:"), 0, AlignMiddle);
    layout->addWidget( _muda_file_edit = new WLineEdit(_muda_file), 0, AlignMiddle);
    _muda_file_edit->setFocus();
    _muda_file_edit->setStyleClass("muda");

    WPushButton* b = new WPushButton("Load");
    layout->addWidget(b, 0, AlignMiddle);
    layout->addStretch();

    b->clicked().connect(bind(&app::startup_muda_screen, this));
    _muda_file_edit->enterPressed().connect(bind(&app::startup_muda_screen, this));
    root()->setLayout(layout);
}

void app::startup_muda_screen()
{
    _muda_file = _muda_file_edit->text().narrow();
    setTitle(_muda_file);
    load_mudas();
    all_view();
}

void app::setup_view()
{
    clear_connections();
    root()->clear();
    create_header_ui();

    _search = _set_search ? _set_search : optional_regex();
    _set_search = optional_regex();
}

void app::all_view()
{
    setup_view();

    //create muda list widget
    mt::muda_list_widget* list_widget = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_all, this, _1), root());
    _connections.push_back(list_widget->when_model_updated(bind(&app::save_mudas, this)));
    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_all_muda, this, list_widget));

}

void app::triage_view()
{
    setup_view();

    //create muda list widget
    mt::muda_list_widget* now = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_now, this, _1), root());
    mt::muda_list_widget* later = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_later, this, _1), root());
    mt::muda_list_widget* done = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_done, this, _1), root());

    _connections.push_back(now->when_model_updated(bind(&app::save_mudas, this)));
    _connections.push_back(later->when_model_updated(bind(&app::save_mudas, this)));
    _connections.push_back(done->when_model_updated(bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_triage_muda, this, now));
}

void app::now_view()
{
    setup_view();

    //create muda list widget
    mt::muda_list_widget* list_widget = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_now, this, _1), root());
    _connections.push_back(list_widget->when_model_updated(bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_now_muda, this, list_widget));
}

void app::later_view()
{
    setup_view();

    //create muda list widget
    mt::muda_list_widget* list_widget = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_later, this, _1), root());
    _connections.push_back(list_widget->when_model_updated(bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_later_muda, this, list_widget));

}

void app::done_view()
{
    setup_view();

    //create muda list widget
    mt::muda_list_widget* list_widget = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_done, this, _1), root());
    _connections.push_back(list_widget->when_model_updated(bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_done_muda, this, list_widget));
}

void app::note_view()
{
    setup_view();

    //create muda list widget
    mt::muda_list_widget* list_widget = new mt::muda_list_widget(_mudas, 
            bind(&app::filter_by_note, this, _1), root());
    _connections.push_back(list_widget->when_model_updated(bind(&app::save_mudas, this)));

    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_note_muda, this, list_widget));
}

void clear_connection(boost::signals2::connection& c)
{
    c.disconnect();
}

void app::clear_connections()
{
    std::for_each(_connections.begin(), _connections.end(), clear_connection);
}

void app::save_mudas()
{
    std::ofstream o(_muda_file.c_str());
    boost::archive::xml_oarchive oa(o);

    oa << BOOST_SERIALIZATION_NVP(_mudas);
}
void app::load_mudas()
{
    std::ifstream i(_muda_file.c_str());
    if(!i) return;

    boost::archive::xml_iarchive ia(i);

    ia >> BOOST_SERIALIZATION_NVP(_mudas);
}

WLabel* create_menu_label(WString text, WString css_class)
{
    WLabel* l = new WLabel(text);
    l->setStyleClass(css_class);
    l->resize(WLength(100, WLength::Percentage), WLength::Auto);
    return l;
}

WContainerWidget* app::create_menu()
{
    WLabel* all = create_menu_label("all", "btn muda-all-button");
    all->clicked().connect (bind(&app::all_view, this));

    WLabel* triage = create_menu_label("triage", "btn muda-all-button");
    triage->clicked().connect (bind(&app::triage_view, this));

    WLabel* now = create_menu_label("now", "btn muda-now-button");
    now->clicked().connect (bind(&app::now_view, this));

    WLabel* later = create_menu_label("later", "btn muda-later-button");
    later->clicked().connect (bind(&app::later_view, this));

    WLabel* done = create_menu_label("done", "btn muda-done-button");
    done->clicked().connect (bind(&app::done_view, this));

    WLabel* note = create_menu_label("note", "btn muda-note-button");
    note->clicked().connect (bind(&app::note_view, this));

    WLabel* menu[] = { all, triage, now, later, done, note};
    unsigned int total = 6;
    unsigned int last = total - 1;

    WContainerWidget* tabs = new WContainerWidget();
    WHBoxLayout* layout = new WHBoxLayout();

    layout->setSpacing(0);

    for(int w = 0; w < total-1; ++w)
    {
        layout->addWidget(menu[w]);
        layout->setStretchFactor(menu[w], 1);
        layout->addSpacing(WLength(8, WLength::Pixel));
    }
    layout->addWidget(menu[last]);
    layout->setStretchFactor(menu[last], 1);
    layout->setContentsMargins(0,0,0,0);

    tabs->setLayout(layout);
    tabs->setStyleClass("menu-container");
    return tabs;
}

void app::create_header_ui()
{
    WContainerWidget* menu = create_menu();

    root()->addWidget(menu);

    _new_muda = new WLineEdit();
    _new_muda->resize(WLength(100, WLength::Percentage), WLength::Auto);
    _new_muda->setStyleClass("new-muda");
    _new_muda->setFocus();

    WHBoxLayout* hbox = new WHBoxLayout();
    hbox->setSpacing(0);
    hbox->addWidget(_new_muda);
    hbox->setContentsMargins(0,0,0,0);

    WContainerWidget* container = new WContainerWidget();
    container->setLayout(hbox);
    root()->addWidget(container);
}

void app::set_search()
{
    BOOST_ASSERT(_new_muda);
    BOOST_ASSERT(_new_muda->text().value().size() > 0);
    std::wstring anyChar(L".*");
    std::wstring search = _new_muda->text().value().substr(1);
    std::wstring regex = anyChar + search + anyChar;
    boost::wregex e(regex);
    _set_search = boost::make_optional(e);
}

void app::clear_search()
{
    _search.reset();
    _set_search.reset();
}

bool app::filter_by_search(mm::muda_ptr muda)
try
{
    BOOST_ASSERT(muda);
    if(!_search) return true;
    return boost::regex_match(muda->text(), _search.get());
}
catch(...)
{
    return false;
}

bool app::filter_by_all(mm::muda_ptr muda)
{
    BOOST_ASSERT(muda);
    return filter_by_search(muda);
}

bool app::filter_by_now(mm::muda_ptr muda)
{
    BOOST_ASSERT(muda);
    return muda->type().state() == m::NOW && filter_by_search(muda);
}

bool app::filter_by_later(mm::muda_ptr muda)
{
    BOOST_ASSERT(muda);
    return muda->type().state() == m::LATER && filter_by_search(muda);
}

bool app::filter_by_done(mm::muda_ptr muda)
{
    BOOST_ASSERT(muda);
    return muda->type().state() == m::DONE && filter_by_search(muda);
}

bool app::filter_by_note(mm::muda_ptr muda)
{
    BOOST_ASSERT(muda);
    return muda->type().state() == m::NOTE && filter_by_search(muda);
}

mm::muda_ptr app::add_new_muda()
{
    BOOST_ASSERT(_new_muda);

    //create new muda and add it to muda list
    mm::muda_ptr muda(new mm::muda);

    mc::modify_muda_text modify_text(*muda, _new_muda->text());
    modify_text();

    mc::add_muda add(muda, _mudas);
    add();
    return muda;
}

bool app::do_search()
{
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
    BOOST_ASSERT(_new_muda);

    if(do_search()) {all_view();return;}

    mm::muda_ptr muda = add_new_muda();
    muda->type().now();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_triage_muda(mt::muda_list_widget* mudas)
{
    BOOST_ASSERT(_new_muda);

    if(do_search()) {triage_view();return;}

    mm::muda_ptr muda = add_new_muda();
    muda->type().now();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_now_muda(mt::muda_list_widget* mudas)
{
    BOOST_ASSERT(_new_muda);

    if(do_search()) {now_view();return;}

    mm::muda_ptr muda = add_new_muda();
    muda->type().now();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_later_muda(mt::muda_list_widget* mudas)
{
    BOOST_ASSERT(_new_muda);

    if(do_search()) {later_view();return;}

    mm::muda_ptr muda = add_new_muda();
    muda->type().later();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_done_muda(mt::muda_list_widget* mudas)
{
    BOOST_ASSERT(_new_muda);

    if(do_search()) {done_view();return;}

    mm::muda_ptr muda = add_new_muda();
    muda->type().done();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_new_note_muda(mt::muda_list_widget* mudas)
{
    BOOST_ASSERT(_new_muda);

    if(do_search()) {note_view();return;}

    mm::muda_ptr muda = add_new_muda();
    muda->type().note();
    add_muda_to_list_widget(muda, mudas);
}

void app::add_muda_to_list_widget(mm::muda_ptr muda, mt::muda_list_widget* list_widget)
{
    BOOST_ASSERT(muda);
    list_widget->add_muda(muda);
    _new_muda->setText(L"");

    save_mudas();
}

WApplication *create_application(const WEnvironment& env)
{
    WApplication* a = new app(env);
    a->useStyleSheet("style.css");
    a->setTitle("Muda");                               
    return a;
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &create_application);
}

