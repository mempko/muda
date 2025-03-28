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

#ifndef MEMPKO_MUDA_WIDGET_H
#define MEMPKO_MUDA_WIDGET_H

#include <iostream>
#include <list>

#include <Wt/WCompositeWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WImage.h>
#include <Wt/WLabel.h>
#include <Wt/WHBoxLayout.h>

#include "core/muda.h"
#include "core/context.h"

namespace mempko::muda::wt { 
    class muda_widget : public Wt::WContainerWidget {
        public:
            muda_widget(
                    dbo::Session& s,
                    model::muda_dptr muda);
            ~muda_widget();

        public:

            using connection = boost::signals2::connection;
            using delete_sig = boost::signals2::signal<void (id_type, muda_widget*)>;
            using delete_slot = delete_sig::slot_type;

            connection when_delete_pressed(const delete_slot& slot);

            using type_sig = boost::signals2::signal<void ()>;
            using type_slot = type_sig::slot_type;
            connection when_type_pressed(const type_slot& slot);

        private:
            void create_ui();
            void change_text();
            void text_changed();
            void update_text();
            void show_buttons();
            void hide_buttons();
            void update_type();
            void type_pressed();
            void delete_pressed();
            void update_date();
            void set_style();

        private:
            model::muda_dptr _muda;
            boost::signals2::connection _when_text_changes;
            boost::signals2::connection _when_type_changes;
            Wt::WContainerWidget* _root;
            Wt::WHBoxLayout* _layout;
            Wt::WLineEdit* _edit;
            Wt::WLabel* _delete_button;
            Wt::WLabel* _date;
            Wt::WLabel* _type;
            delete_sig _delete_sig;
            type_sig _type_sig;
            dbo::Session& _session;
            bool _dirty = false;
            bool _enter = false;
    };

    using muda_widget_list = std::list<muda_widget*>;

}
#endif
