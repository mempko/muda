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

#ifndef MEMPKO_MUDA_LIST_WIDGET_H
#define MEMPKO_MUDA_LIST_WIDGET_H

#include <boost/function.hpp>

#include <Wt/WCompositeWidget>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WImage>
#include <Wt/WLabel>
#include <Wt/WHBoxLayout>

#include "wtui/mudawidget.h"

namespace mempko { namespace muda { namespace wt { 

    typedef std::list<boost::signals2::connection> connections;

    class muda_list_widget : public Wt::WCompositeWidget
    {
        public:
            typedef boost::function<bool (model::muda_ptr)> filter_func;
            muda_list_widget(
                    model::muda_list& mudas, 
                    filter_func filter, 
                    Wt::WContainerWidget* parent = 0);
            ~muda_list_widget();
        public:
            typedef boost::signals2::connection connection;
            typedef boost::signals2::signal<void ()> update_sig;
            typedef update_sig::slot_type update_slot;
            connection when_model_updated(const update_slot& slot);
        public:
            void add_muda(model::muda_ptr muda);
        private:
            void create_ui();
            void remove_muda(id_type id, muda_widget* widget);
            void fire_update_sig();
            void clear_connections();
        private:
            filter_func _filter;
            model::muda_list& _mudas;
            muda_widget_list _muda_widgets;
            update_sig _update_sig;
            Wt::WContainerWidget* _root;

            connections _connections;
    };

}}}//namespace
#endif
