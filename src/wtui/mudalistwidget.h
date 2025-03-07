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

#ifndef MEMPKO_MUDA_LIST_WIDGET_H
#define MEMPKO_MUDA_LIST_WIDGET_H

#include <functional>

#include <Wt/WCompositeWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WImage.h>
#include <Wt/WLabel.h>
#include <Wt/WHBoxLayout.h>
#include <memory>

#include "wtui/mudawidget.h"

namespace mempko::muda::wt { 
    using connections = std::list<boost::signals2::connection>;
    using muda_vec = std::vector<model::muda_dptr>;

    class muda_list_widget : public Wt::WCompositeWidget {
        public:
            using mutate_func = std::function<void (muda_vec&)>;
            muda_list_widget(
                    dbo::Session& s,
                    model::muda_list_dptr mudas, 
                    mutate_func mut);
            ~muda_list_widget();

        public:
            using connection = boost::signals2::connection;
            using update_sig = boost::signals2::signal<void ()>;
            using update_slot = update_sig::slot_type;

            auto when_model_updated(const update_slot& slot) {
                return _update_sig.connect(slot);
            }

        public:
            void add_muda(model::muda_dptr muda);

        private:
            void create_ui(mutate_func);
            void remove_muda(id_type id, muda_widget* widget);
            void fire_update_sig();
            void clear_connections();

        private:
            model::muda_list_dptr _mudas;
            muda_widget_list _muda_widgets;
            update_sig _update_sig;
            Wt::WContainerWidget* _root;
            dbo::Session& _session;

            connections _connections;
    };
}
#endif
