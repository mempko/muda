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

#include "wtui/mudalistwidget.h"
#include <memory>

namespace mempko::muda::wt { 
    namespace w = Wt;

    muda_list_widget::muda_list_widget(
            dbo::Session& s,
            model::muda_list_dptr mudas, 
            muda_list_widget::mutate_func mut) :
        w::WCompositeWidget{},
        _mudas{mudas}, _session{s} {
        auto root = std::make_unique<w::WContainerWidget>();
        _root = root.get();
        setImplementation(std::move(root));
        create_ui(mut);
    }

    muda_list_widget::~muda_list_widget() {
        clear_connections();
    }

    void muda_list_widget::clear_connections() {
        for(auto c : _connections) c.disconnect();
        _connections.clear();
    }

    void muda_list_widget::create_ui(muda_list_widget::mutate_func mut) {
        dbo::Transaction t{_session};
        _root->resize(w::WLength(100, w::WLength::Unit::Percentage), w::WLength::Auto);

        const auto& list = _mudas->list();

        auto vec = muda_vec{std::begin(list), std::end(list)};
        mut(vec);

        for(const auto& m : vec) add_muda(m);
    }

    void muda_list_widget::add_muda(model::muda_dptr muda) {
        REQUIRE(muda);

        _connections.push_back(muda.modify()->when_text_changes(
                    [this]() {
                        fire_update_sig();
                    }));

        //create new muda widget and add it to widget list
        auto new_widget = std::make_unique<muda_widget>(_session, muda);
        auto new_widget_p = new_widget.get();

        _connections.push_back(new_widget->when_delete_pressed(
                    [this](auto id, auto widget) { 
                        remove_muda(id, widget);
                    }));

        _connections.push_back(new_widget->when_type_pressed(
                    [this]() {
                        fire_update_sig();
                    }));


        if(_muda_widgets.empty()) {
            _root->addWidget(std::move(new_widget));
        } else {
            _root->insertBefore(std::move(new_widget), _muda_widgets.back());
        }

        _muda_widgets.push_back(new_widget_p);
    }

    void muda_list_widget::remove_muda(id_type id, muda_widget* widget) {
        REQUIRE(widget);
        dbo::Transaction t{_session};

        auto remove = context::remove_muda{id, *(_mudas.modify())}; 
        remove();

        fire_update_sig();

        _muda_widgets.remove(widget);
        _root->removeWidget(widget);
    }

    void muda_list_widget::fire_update_sig() {
        INVARIANT(this);
        _update_sig();
    }
}
