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

#include "wtui/mudalistwidget.h"

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            using boost::bind;
            using boost::mem_fn;
            namespace w = Wt;

            muda_list_widget::muda_list_widget(
                    dbo::Session& s,
                    model::muda_list_dptr mudas, 
                    muda_list_widget::filter_func filter, 
                    w::WContainerWidget* parent) :
                w::WCompositeWidget{parent},
                _mudas{mudas}, _filter{filter}, _session{s}
            {
                REQUIRE(parent);

                setImplementation(_root = new w::WContainerWidget);
                create_ui();
            }

            muda_list_widget::~muda_list_widget()
            {
                clear_connections();
            }

            void muda_list_widget::clear_connections()
            {
                for(auto c : _connections) c.disconnect();
                _connections.clear();
            }

            void muda_list_widget::create_ui()
            {
                dbo::Transaction t{_session};
                _root->resize(w::WLength(100, w::WLength::Percentage), w::WLength::Auto);

                const auto& list = _mudas->list();

                auto ordered_list = std::vector<model::muda_dptr>{std::begin(list), std::end(list)};

                std::sort(std::begin(ordered_list), std::end(ordered_list), 
                        [](const auto& a, const auto& b)
                        { 
                            return a->id() < b->id();
                        });

                for(const auto& m : ordered_list) add_muda(m);
            }

            void muda_list_widget::add_muda(model::muda_dptr muda)
            {
                REQUIRE(muda);

                //do not add if it does not pass filter
                if(!_filter(muda)) return;

                _connections.push_back(muda.modify()->when_text_changes(bind(&muda_list_widget::fire_update_sig, this)));

                //create new muda widget and add it to widget list
                auto new_widget = new muda_widget{_session, muda};

                _connections.push_back(new_widget->when_delete_pressed(bind(&muda_list_widget::remove_muda, this, _1, _2)));
                _connections.push_back(new_widget->when_type_pressed(bind(&muda_list_widget::fire_update_sig, this)));

                if(_muda_widgets.empty()) _root->addWidget(new_widget);
                else _root->insertBefore(new_widget, _muda_widgets.back());

                _muda_widgets.push_back(new_widget);
            }

            void muda_list_widget::remove_muda(id_type id, muda_widget* widget)
            {
                REQUIRE(widget);
                dbo::Transaction t{_session};

                auto remove = context::remove_muda{id, *(_mudas.modify())}; 
                remove();

                fire_update_sig();
                _muda_widgets.remove(widget);
                _root->removeWidget(widget);
            }

            void muda_list_widget::fire_update_sig()
            {
                INVARIANT(this);
                _update_sig();
            }
        }
    }
}
