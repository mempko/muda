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

#include "wtui/romudalistwidget.h"

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 
            using boost::bind;
            using boost::mem_fn;
            namespace w = Wt;

            ro_muda_list_widget::ro_muda_list_widget(
                    dbo::Session& s,
                    model::muda_list_dptr mudas, 
                    ro_muda_list_widget::mutate_func mut, 
                    w::WContainerWidget* parent) :
                w::WCompositeWidget{parent},
                _mudas{mudas}, _session{s}
            {
                REQUIRE(parent);

                setImplementation(_root = new w::WContainerWidget);
                create_ui(mut);
            }

            ro_muda_list_widget::~ro_muda_list_widget() {}

            void ro_muda_list_widget::create_ui(ro_muda_list_widget::mutate_func mut)
            {
                dbo::Transaction t{_session};
                _root->resize(w::WLength(100, w::WLength::Percentage), w::WLength::Auto);

                const auto& list = _mudas->list();

                auto vec = muda_vec{std::begin(list), std::end(list)};
                mut(vec);

                for(const auto& m : vec) add_muda(m);
            }

            void ro_muda_list_widget::add_muda(model::muda_dptr muda)
            {
                REQUIRE(muda);

                //create new muda widget and add it to widget list
                auto new_widget = new ro_muda_widget{_session, muda};

                if(_muda_widgets.empty()) _root->addWidget(new_widget);
                else _root->insertBefore(new_widget, _muda_widgets.back());

                _muda_widgets.push_back(new_widget);
            }
        }
    }
}
