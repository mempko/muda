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

#include "wtui/romudawidget.h"

#include "core/dbc.h"

#include <Wt/WLength>
#include <sstream>
#include <chrono>
#include <ctime>

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 

            using boost::bind;
            using boost::mem_fn;
            namespace w = Wt;

            namespace
            {
                std::string timestamp(time pt)
                {
                    using namespace boost::posix_time;
                    using namespace boost::gregorian;

                    time epoch{boost::gregorian::date(1970, 1, 1)};
                    auto x = (pt - epoch).total_seconds();
                    auto t = time_t(x);

                    char str[20];
                    if(std::strftime(str, sizeof(str), "%b %d %I:%M:%S", std::localtime(&t))) 
                        return std::string(str);

                    return "";
                }
            }

            ro_muda_widget::ro_muda_widget(
                    dbo::Session& s,
                    model::muda_dptr muda, 
                    w::WContainerWidget* parent) :
                w::WContainerWidget{parent},
                _muda{muda}, _session{s}
            {
                REQUIRE(muda);

                create_ui();

                ENSURE(_muda);
            }

            ro_muda_widget::~ro_muda_widget() {}

            void ro_muda_widget::create_ui()
            {
                INVARIANT(_muda);

                //create text label
                _text = new w::WLabel;
                _text->setText(_muda->text());
                _text->setWordWrap(true);

                //date 
                _date = new w::WLabel{""};

                //layout
                _layout = new w::WHBoxLayout();
                _layout->setSpacing(8);
                _layout->addWidget(_text);
                _layout->setStretchFactor(_text, 1);
                _layout->addWidget(_date);
                _layout->setContentsMargins(0,0,0,0);

                setLayout(_layout);
                setStyleClass("muda-container");
                resize(w::WLength(100, w::WLength::Percentage), w::WLength::Auto);

                set_style();
                set_date();

                ENSURE(_text);
                ENSURE(_layout);
                ENSURE(_date);
            }

            void ro_muda_widget::set_style()
            {
                INVARIANT(_muda);
                INVARIANT(_date);

                switch(_muda->type().state())
                {
                    case NOW: 
                        _text->setStyleClass("muda-now");
                        _date->setStyleClass("timestamp muda-now");
                        break;
                    case LATER: 
                        _text->setStyleClass("muda-later");
                        _date->setStyleClass("timestamp muda-later");

                        break;
                    case DONE: 
                        _text->setStyleClass("muda-done");
                        _date->setStyleClass("timestamp muda-done");

                        break;
                    case NOTE: 
                        _text->setStyleClass("muda-note");
                        _date->setStyleClass("timestamp muda-note");

                        break;
                }
            }

            void ro_muda_widget::set_date()
            {
                INVARIANT(_date);
                INVARIANT(_muda);

                auto text_modified_date = _muda->modified();
                auto type_modified_date = _muda->type().modified();

                CHECK_FALSE(text_modified_date.is_not_a_date_time());
                
                auto date = type_modified_date.is_not_a_date_time() ? 
                    text_modified_date :
                    std::max(text_modified_date, type_modified_date);

                _date->setText(timestamp(date));
            }
        }
    }
}
