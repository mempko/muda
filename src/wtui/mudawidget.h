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

#ifndef MEMPKO_MUDA_WIDGET_H
#define MEMPKO_MUDA_WIDGET_H

#include <iostream>
#include <list>

#include <Wt/WCompositeWidget>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WImage>
#include <Wt/WLabel>
#include <Wt/WHBoxLayout>

#include "core/muda.h"
#include "core/context.h"

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 

            class muda_widget : public Wt::WCompositeWidget
            {
                public:
                    muda_widget(
                            dbo::Session& s,
                            model::muda_dptr muda, 
                            Wt::WContainerWidget* parent = 0);
                    ~muda_widget();

                public:
                    typedef boost::signals2::connection connection;
                    typedef boost::signals2::signal<void (id_type, muda_widget*)> delete_sig;
                    typedef delete_sig::slot_type delete_slot;
                    connection when_delete_pressed(const delete_slot& slot);

                    typedef boost::signals2::signal<void ()> type_sig;
                    typedef type_sig::slot_type type_slot;
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
                    void set_style();

                private:
                    model::muda_dptr _muda;
                    boost::signals2::connection _when_text_changes;
                    boost::signals2::connection _when_type_changes;
                    Wt::WContainerWidget* _root;
                    Wt::WHBoxLayout* _layout;
                    Wt::WLineEdit* _edit;
                    Wt::WLabel* _delete_button;
                    Wt::WLabel* _type;
                    delete_sig _delete_sig;
                    type_sig _type_sig;
                    dbo::Session& _session;
                    bool _dirty = false;
                    bool _enter = false;

            };

            typedef std::list<muda_widget*> muda_widget_list;

        }
    }
}

#endif
