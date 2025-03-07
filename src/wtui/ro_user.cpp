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

#include "wtui/ro_user.h"
#include <memory>

using namespace Wt;
namespace ph = std::placeholders;

namespace mm = mempko::muda::model;
namespace mu = mempko::muda::util;

namespace mempko::muda::wt { 
    const std::string RO_APP_PATH = "/u";

    namespace {
        void muda_list_sort(muda_vec& v) {
            std::sort(std::begin(v), std::end(v), 
                    [](const auto& a, const auto& b) { return a->id() < b->id();});
        }

        std::unique_ptr<WLabel> create_small_label(WString text, WString css_class) {
            auto l = std::make_unique<WLabel>(text);
            l->setStyleClass(css_class);

            ENSURE(l);
            return l;
        }

        std::unique_ptr<WLabel> create_menu_label(WString text, WString css_class) {
            auto l = std::make_unique<WLabel>(text);
            l->setStyleClass(css_class);
            l->resize(WLength(100, WLength::Unit::Percentage), WLength::Auto);

            ENSURE(l);
            return l;
        }

    }

    ro_user::ro_user(const WEnvironment& env) : WApplication{env} {
        std::string db;

        if(!env.server()->readConfigurationProperty("db", db)) {
            throw std::invalid_argument("db configuration is missing");
        }

        _session = std::make_shared<wt::session>(db);
        _session->login().changed().connect(this, &ro_user::oauth_event);

        internalPathChanged().connect(this, &ro_user::handle_path);

        set_view();

        ENSURE(_session);
    }

    void ro_user::handle_path() {
        set_view();
    }

    void ro_user::set_view() {
        INVARIANT(_session);

        load_user();

        if(_user) {
            startup_muda_screen();
        } else {
            setInternalPath("../");
        }
    }

    void ro_user::oauth_event() {
        INVARIANT(_session);

        set_view();
    }

    void ro_user::startup_muda_screen() {
        INVARIANT(root());
        INVARIANT(_session);
        INVARIANT(_user);
        INVARIANT(_mudas);
        INVARIANT(_mudas->is_public());

        root()->clear();

        setTitle(_user_name);
        triage_view();
    }

    void ro_user::setup_view() {
        INVARIANT(root());

        if(_search_box) {
            do_search();
        }

        root()->clear();
        create_header_ui();

        _search = _set_search;
    }

    void ro_user::triage_view() {
        INVARIANT(root());
        INVARIANT(_session);

        setup_view();

        //create muda list widget
        auto triage = std::make_unique<ro_muda_list_widget>(
                _session->dbs(),
                _mudas, 
                [this](muda_vec& mudas) {
                    make_prioritize_view(mudas);
                });

        root()->addWidget(std::move(triage));

        _search_box->enterPressed().connect(
                [this]() {
                    triage_view();
                });
    }

    void ro_user::now_view() {
        INVARIANT(root());
        INVARIANT(_session);

        setup_view();

        //create muda list widget
        auto list_widget = std::make_unique<ro_muda_list_widget>(
                _session->dbs(),
                _mudas, 
                [this](muda_vec& mudas) {
                    make_now_view(mudas);
                });

        root()->addWidget(std::move(list_widget));

        _search_box->enterPressed().connect(
                [this]() {
                    now_view();
                });
    }

    void ro_user::later_view() {
        INVARIANT(root());
        INVARIANT(_session);

        setup_view();

        //create muda list widget
        auto list_widget = std::make_unique<ro_muda_list_widget>(
                _session->dbs(),
                _mudas, 
                [this](muda_vec& mudas) {
                    make_later_view(mudas);
                });

        root()->addWidget(std::move(list_widget));

        _search_box->enterPressed().connect(
                [this]() {
                    later_view();
                });
    }

    void ro_user::done_view() {
        INVARIANT(root());
        INVARIANT(_session);

        setup_view();

        //create muda list widget
        auto list_widget = std::make_unique<ro_muda_list_widget>(
                _session->dbs(),
                _mudas, 
                [this](muda_vec& mudas) {
                    make_done_view(mudas);
                });

        root()->addWidget(std::move(list_widget));

        _search_box->enterPressed().connect(
                [this]() {
                    done_view();
                });
    }

    void ro_user::note_view() {
        INVARIANT(_session);

        setup_view();

        //create muda list widget
        auto list_widget = std::make_unique<ro_muda_list_widget>(
                _session->dbs(),
                _mudas, 
                [this](muda_vec& mudas) {
                    make_note_view(mudas);
                });

        root()->addWidget(std::move(list_widget));

        _search_box->enterPressed().connect(
                [this]() {
                    note_view();
                });
    }

    void ro_user::load_user() {
        INVARIANT(_session);

        _user.reset();
        _mudas.reset();

        dbo::Transaction t{_session->dbs()};

        _user_name = internalPathNextPart("/");
        Wt::log("info") << "RO USER: " << _user_name; 

        _user = _session->ro_user(_user_name);
        if(!_user) {
            return;
        }

        Wt::log("info") << "FOUND USER: " << _user_name;

        auto& lists = _user.modify()->lists();
        CHECK_GREATER(lists.size(), 0)

            _mudas = lists.front();
        CHECK(_mudas);

        //if list isn't public, reset
        if(!_mudas->is_public()) {
            _user.reset();
            _mudas.reset();
        }
    }

    std::unique_ptr<WContainerWidget> ro_user::create_menu() {
        //setup menu
        auto triage = create_menu_label("prioritize", "btn muda-pri-button");
        triage->clicked().connect ([this]() { triage_view(); });

        auto now = create_menu_label("now", "btn muda-now-button");
        now->clicked().connect ([this]() { now_view(); });

        auto later = create_menu_label("later", "btn muda-later-button");
        later->clicked().connect ([this]() { later_view(); });

        auto done = create_menu_label("done", "btn muda-done-button");
        done->clicked().connect ([this]() { done_view(); });

        auto note = create_menu_label("note", "btn muda-note-button");
        note->clicked().connect ([this]() { note_view(); });

        std::vector<std::unique_ptr<WLabel>*> menu {
            &triage,
            &now,
            &later,
            &done,
            &note
        };

        auto tabs = std::make_unique<WContainerWidget>();
        auto layout = std::make_unique<WHBoxLayout>();

        layout->setSpacing(0);

        for(auto m : menu) {
            auto mp = m->get();
            layout->addWidget(std::move(*m));
            layout->setStretchFactor(mp, 1);
            layout->addSpacing(WLength(8, WLength::Unit::Pixel));
        }

        layout->setContentsMargins(0,0,0,0);

        tabs->setLayout(std::move(layout));
        tabs->setStyleClass("menu-container");

        ENSURE(tabs);
        return tabs;
    }

    void ro_user::create_header_ui() {
        INVARIANT(root());

        auto menu = create_menu();

        root()->addWidget(std::move(menu));

        auto search_box = std::make_unique<WLineEdit>();
        _search_box = search_box.get();
        search_box->setStyleClass("new-muda");
        search_box->setFocus();

        auto hbox = std::make_unique<WHBoxLayout>();
        hbox->setSpacing(0);
        hbox->addWidget(std::move(search_box));
        hbox->setContentsMargins(0,0,0,0);

        auto container = std::make_unique<WContainerWidget>();
        container->setLayout(std::move(hbox));
        root()->addWidget(std::move(container));

        ENSURE(_search_box)
    }

    void ro_user::set_search() try {
        INVARIANT(_search_box);
        INVARIANT_GREATER(_search_box->text().value().size(), 0);

        std::string anyChar{".*"};
        std::string search = _search_box->text().toUTF8();
        std::string regex = anyChar + search + anyChar;

        std::regex e{regex};
        _set_search = e;
    } catch(std::exception& e) {
        Wt::log("error") << "regex error" << e.what();
    }

    void ro_user::clear_search() {
        _search.reset();
        _set_search.reset();
    }

    bool ro_user::filter_by_search(mm::muda_dptr muda) try {
        REQUIRE(muda);

        if(!_search) {
            return true;
        }
        return std::regex_match(muda->text(), _search.value());
    } catch(...) {
        return false;
    }

    bool ro_user::filter_by_now(mm::muda_dptr muda) {
        REQUIRE(muda);
        return muda->type().state() == muda_state::NOW && filter_by_search(muda);
    }

    bool ro_user::filter_by_later(mm::muda_dptr muda) {
        REQUIRE(muda);
        return muda->type().state() == muda_state::LATER && filter_by_search(muda);
    }

    bool ro_user::filter_by_done(mm::muda_dptr muda) {
        REQUIRE(muda);
        return muda->type().state() == muda_state::DONE && filter_by_search(muda);
    }

    bool ro_user::filter_by_note(mm::muda_dptr muda) {
        REQUIRE(muda);
        return muda->type().state() == muda_state::NOTE && filter_by_search(muda);
    }

    void ro_user::make_now_view(muda_vec& v) {
        mu::filter(v, [&](auto muda) { return this->filter_by_now(muda);});
        muda_list_sort(v);
    }

    void ro_user::make_later_view(muda_vec& v) {
        mu::filter(v, [&](auto muda) { return this->filter_by_later(muda);});
        muda_list_sort(v);
    }

    void ro_user::make_done_view(muda_vec& v) {
        mu::filter(v, [&](auto muda) { return this->filter_by_done(muda);});
        muda_list_sort(v);
    }

    void ro_user::make_note_view(muda_vec& v) {
        mu::filter(v, [&](auto muda) { return this->filter_by_note(muda);});
        muda_list_sort(v);
    }

    void ro_user::make_prioritize_view(muda_vec& v) {
        //filter
        mu::filter(v,
                [&](auto muda) { 
                    return muda->type().state() != muda_state::NOTE &&
                        this->filter_by_search(muda);
                }); 

        muda_list_sort(v);

        //put done first, then later, then now
        //This order is important because it will show up in reverse when added to the muda list
        std::stable_sort(
                v.begin(), v.end(), [](auto a, auto b) { 
                    return static_cast<int>(a->type().state()) >
                        static_cast<int>(b->type().state());
                });
    }

    bool ro_user::do_search() {
        INVARIANT(_search_box);

        auto mt = _search_box->text().value();
        if(!mt.empty()) {
            set_search();
            return true;
        }
        clear_search();
        return mt.size() == 0;
    }

    std::unique_ptr<WApplication> create_ro_user_application(const WEnvironment& env) {
        auto a = std::make_unique<ro_user>(env);

        a->useStyleSheet("resources/style.css");
        a->useStyleSheet("resources/fonts/entypo.css");
        a->setTitle("Muda");                               
        a->messageResourceBundle().use(a->appRoot() + "/templates");

        return a;
    }
}
