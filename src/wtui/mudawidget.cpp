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

#include "wtui/mudawidget.h"
#include "core/dbc.h"

#include <Wt/WLength.h>
#include <sstream>
#include <chrono>
#include <ctime>

namespace mempko::muda::wt 
{ 
    using boost::bind;
    using boost::mem_fn;
    namespace w = Wt;

    namespace
    {
        std::string timestamp(time pt)
        {
            auto x = pt.time_since_epoch()* std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;
            auto t = time_t(x.count());

            char str[20];
            if(std::strftime(str, sizeof(str), "%b %d %I:%M:%S", std::localtime(&t))) 
                return std::string(str);

            return "";
        }
    }

    muda_widget::muda_widget(
            dbo::Session& s,
            model::muda_dptr muda) :
        w::WContainerWidget{},
        _muda{muda}, _session{s}
    {
        REQUIRE(muda);

        create_ui();

        ENSURE(_muda);
    }

    muda_widget::~muda_widget()
    {
        _when_text_changes.disconnect();
        _when_type_changes.disconnect();
    }

    void muda_widget::create_ui()
    {
        INVARIANT(_muda);

        //create text editor
        auto edit = std::make_unique<w::WLineEdit>();
        _edit = edit.get();

        //set text to muda text
        edit->setText(_muda->text());

        //when enter is pressed change the model
        edit->keyWentDown().connect(this, &muda_widget::text_changed);
        edit->enterPressed().connect(this, &muda_widget::change_text);

        //when the model changes the text or type
        _when_text_changes = _muda.modify()->when_text_changes(
                bind(&muda_widget::update_text, this));
        _when_type_changes = _muda.modify()->type().when_type_changes(
                bind(&muda_widget::update_type, this));

        //type button
        auto type = std::make_unique<w::WLabel>("now");
        _type = type.get();
        type->clicked().connect(this, &muda_widget::type_pressed);

        //date label
        auto date = std::make_unique<w::WLabel>("");
        _date = date.get();

        //delete button
        auto delete_button = std::make_unique<w::WLabel>("x");
        _delete_button = delete_button.get();
        _delete_button->setStyleClass("btn muda-delete");
        _delete_button->setToolTip("double-click to erase");
        _delete_button->doubleClicked().connect(this, &muda_widget::delete_pressed);


        update_type();
        show_buttons();

        //layout
        auto layout = std::make_unique<w::WHBoxLayout>();
        _layout = layout.get();
        layout->setSpacing(8);
        layout->addWidget(std::move(edit));
        layout->setStretchFactor(_edit, 1);
        layout->addWidget(std::move(type));
        layout->addWidget(std::move(date));
        layout->addWidget(std::move(delete_button));
        layout->setContentsMargins(0,0,0,0);

        setLayout(std::move(layout));
        setStyleClass("muda-container");
        resize(w::WLength(100, w::WLength::Unit::Percentage), w::WLength::Auto);

        ENSURE(_edit);
        ENSURE(_layout);
        ENSURE(_delete_button);
        ENSURE(_type);
        ENSURE(_date);
    }

    void muda_widget::text_changed()
    {
        INVARIANT(_muda);
        INVARIANT(_edit);
        if(_dirty) return;
        if(_enter) 
        {
            _enter = false;
            return;
        }

        _dirty = true;
        set_style();
    }

    void muda_widget::change_text()
    {
        INVARIANT(_muda);
        INVARIANT(_edit);

        if(_dirty)
        {
            _dirty = false;
            set_style();
        }
        _enter = true;

        dbo::Transaction t{_session};

        auto modify_text = context::modify_muda_text{*(_muda.modify()), _edit->text().toUTF8()};
        modify_text();

    }

    void muda_widget::update_text()
    {
        INVARIANT(_muda);

        _edit->setText(_muda->text());
        update_date();
    }

    void muda_widget::show_buttons()
    {
        _date->show();
        _delete_button->show();
        _type->show();
    }

    void muda_widget::hide_buttons()
    {
        _date->hide();
        _delete_button->hide();
        _type->hide();

    }

    void muda_widget::delete_pressed()
    {
        INVARIANT(_muda);

        hide_buttons();
        _delete_sig(_muda->id(), this);
    }

    boost::signals2::connection muda_widget::when_delete_pressed(const delete_slot& slot) 
    { 
        return _delete_sig.connect(slot);
    }

    boost::signals2::connection muda_widget::when_type_pressed(const type_slot& slot) 
    { 
        return _type_sig.connect(slot);
    }

    void muda_widget::type_pressed()
    {
        INVARIANT(_muda);
        dbo::Transaction t{_session};

        auto transition = context::transition_state{_muda.modify()->type()};
        transition();
        _type_sig();
    }

    void muda_widget::set_style()
    {
        INVARIANT(_muda);
        INVARIANT(_type);
        INVARIANT(_date);

        switch(_muda->type().state())
        {
            case muda_state::NOW: 
                _type->setStyleClass("btn muda-now-button");
                _date->setStyleClass("timestamp muda-now");

                if(_dirty) _edit->setStyleClass("dirty muda-now");
                else  _edit->setStyleClass("muda-now");

                break;
            case muda_state::LATER: 
                _type->setStyleClass("btn muda-later-button");
                _date->setStyleClass("timestamp muda-later");

                if(_dirty) _edit->setStyleClass("dirty muda-later");
                else _edit->setStyleClass("muda-later");

                break;
            case muda_state::DONE: 
                _type->setStyleClass("btn muda-done-button");
                _date->setStyleClass("timestamp muda-done");

                if(_dirty) _edit->setStyleClass("dirty muda-done");
                else _edit->setStyleClass("muda-done");

                break;
            case muda_state::NOTE: 
                _type->setStyleClass("btn muda-note-button");
                _date->setStyleClass("timestamp muda-note");

                if(_dirty) _edit->setStyleClass("dirty muda-note");
                else _edit->setStyleClass("muda-note");
                break;
        }
    }

    void muda_widget::update_date()
    {
        INVARIANT(_date);
        INVARIANT(_muda);

        auto text_modified_date = _muda->modified();
        auto type_modified_date = _muda->type().modified();

        auto date = std::max(text_modified_date, type_modified_date);

        _date->setText(timestamp(date));
    }

    void muda_widget::update_type()
    {
        INVARIANT(_muda);
        INVARIANT(_type);

        switch(_muda->type().state())
        {
            case muda_state::NOW: 
                _type->setText("now");
                break;
            case muda_state::LATER: 
                _type->setText("later");
                break;
            case muda_state::DONE: 
                _type->setText("done");
                break;
            case muda_state::NOTE: 
                _type->setText("note");
                break;
        }

        update_date();
        set_style();
    }
}
