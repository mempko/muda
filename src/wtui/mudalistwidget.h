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

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 

            typedef std::list<boost::signals2::connection> connections;
            using muda_vec = std::vector<model::muda_dptr>;

            class muda_list_widget : public Wt::WCompositeWidget
            {
                public:
                    typedef boost::function<void (muda_vec&)> mutate_func;
                    muda_list_widget(
                            dbo::Session& s,
                            model::muda_list_dptr mudas, 
                            mutate_func mut, 
                            Wt::WContainerWidget* parent = 0);
                    ~muda_list_widget();

                public:
                    typedef boost::signals2::connection connection;
                    typedef boost::signals2::signal<void ()> update_sig;
                    typedef update_sig::slot_type update_slot;

                    auto when_model_updated(const update_slot& slot)
                    {
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
    }
}
#endif
