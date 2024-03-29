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

#ifndef MEMPKO_MUDA_RO_LIST_WIDGET_H
#define MEMPKO_MUDA_RO_LIST_WIDGET_H

#include <boost/function.hpp>

#include <Wt/WCompositeWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WImage.h>
#include <Wt/WLabel.h>
#include <Wt/WHBoxLayout.h>

#include "wtui/romudawidget.h"

namespace mempko::muda::wt
{ 
    using muda_vec = std::vector<model::muda_dptr>;

    class ro_muda_list_widget : public Wt::WCompositeWidget
    {
        public:
            typedef boost::function<void (muda_vec&)> mutate_func;
            ro_muda_list_widget(
                    dbo::Session& s,
                    model::muda_list_dptr mudas, 
                    mutate_func mut);
            ~ro_muda_list_widget();

        public:
            void add_muda(model::muda_dptr muda);

        private:
            void create_ui(mutate_func);

        private:
            model::muda_list_dptr _mudas;
            ro_muda_widget_list _muda_widgets;
            Wt::WContainerWidget* _root;
            dbo::Session& _session;
    };
}
#endif
