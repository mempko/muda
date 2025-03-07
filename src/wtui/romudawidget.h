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

#ifndef MEMPKO_MUDA_RO_WIDGET_H
#define MEMPKO_MUDA_RO_WIDGET_H

#include <iostream>
#include <list>

#include <Wt/WCompositeWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WImage.h>
#include <Wt/WLabel.h>
#include <Wt/WHBoxLayout.h>

#include "core/muda.h"

namespace mempko::muda::wt { 
    class ro_muda_widget : public Wt::WContainerWidget
    {
        public:
            ro_muda_widget(
                    dbo::Session& s,
                    model::muda_dptr muda);
            ~ro_muda_widget();

        private:
            void create_ui();
            void set_style();
            void set_date();

        private:
            model::muda_dptr _muda;
            Wt::WContainerWidget* _root;
            Wt::WHBoxLayout* _layout;
            Wt::WLabel* _text;
            Wt::WLabel* _date;
            dbo::Session& _session;
    };

    using ro_muda_widget_list = std::list<ro_muda_widget*>;

}

#endif
