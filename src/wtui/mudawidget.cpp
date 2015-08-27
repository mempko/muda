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

#include "wtui/mudawidget.h"
#include "core/dbc.h"

namespace mempko 
{ 
    namespace muda 
    { 
        namespace wt 
        { 

            using boost::bind;
            using boost::mem_fn;
            namespace w = Wt;

            muda_widget::muda_widget(
                    dbo::Session& s,
                    model::muda_dptr muda, 
                    w::WContainerWidget* parent) :
                w::WCompositeWidget{parent},
                _muda{muda}, _session{s}
            {
                REQUIRE(muda);

                setImplementation(_root = new w::WContainerWidget);
                create_ui();

                ENSURE(_root);
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
                INVARIANT(_root);

                //create text editor
                _edit = new w::WLineEdit;
                //set text to muda text
                _edit->setText(_muda->text());

                //when enter is pressed change the model
                _edit->enterPressed().connect
                    (bind(&muda_widget::change_text, this));

                //when the model changes the text or type
                _when_text_changes = _muda.modify()->when_text_changes(
                        bind(&muda_widget::update_text, this));
                _when_type_changes = _muda.modify()->type().when_type_changes(
                        bind(&muda_widget::update_type, this));

                //delete button
                _deleteButton = new w::WLabel{"x"};
                _deleteButton->setStyleClass("btn muda-delete");
                _deleteButton->doubleClicked().connect
                    (bind(&muda_widget::delete_pressed, this));

                //type button
                _type = new w::WLabel{"now"};
                _type->clicked().connect
                    (bind(&muda_widget::type_pressed, this));
                update_type();
                hide_buttons();

                //layout
                _layout = new w::WHBoxLayout();
                _layout->setSpacing(5);
                _layout->addWidget(_edit);
                _layout->setStretchFactor(_edit, 1);
                _layout->addWidget(_type);
                _layout->addWidget(_deleteButton);
                _layout->setContentsMargins(0,0,0,0);

                _root->setLayout(_layout);
                _root->setStyleClass("muda-container");
                _root->resize(w::WLength(100, w::WLength::Percentage), w::WLength::Auto);

                implementStateless(&muda_widget::show_buttons);
                implementStateless(&muda_widget::hide_buttons);

                _root->mouseWentOver().connect
                    (bind(&muda_widget::show_buttons, this));
                _root->mouseWentOut().connect
                    (bind(&muda_widget::hide_buttons, this));

                ENSURE(_edit);
                ENSURE(_layout);
                ENSURE(_deleteButton);
                ENSURE(_type);
            }

            void muda_widget::change_text()
            {
                INVARIANT(_muda);
                INVARIANT(_edit);
                dbo::Transaction t{_session};

                auto modify_text = context::modify_muda_text{*(_muda.modify()), _edit->text().narrow()};
                modify_text();
            }

            void muda_widget::update_text()
            {
                INVARIANT(_muda);

                _edit->setText(_muda->text());
            }

            void muda_widget::show_buttons()
            {
                INVARIANT(_deleteButton);
                INVARIANT(_type);

                _deleteButton->setHidden(false);
                _type->setHidden(false);
            }

            void muda_widget::hide_buttons()
            {
                INVARIANT(_deleteButton);
                INVARIANT(_type);

                _deleteButton->setHidden(true);
                _type->setHidden(true);
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

            void muda_widget::update_type()
            {
                INVARIANT(_muda);
                INVARIANT(_type);

                switch(_muda->type().state())
                {
                    case NOW: 
                        _type->setText("now");
                        _type->setStyleClass("btn muda-now-button");
                        _edit->setStyleClass("muda-now");
                        break;
                    case LATER: 
                        _type->setText("later");
                        _type->setStyleClass("btn muda-later-button");
                        _edit->setStyleClass("muda-later");
                        break;
                    case DONE: 
                        _type->setText("done");
                        _type->setStyleClass("btn muda-done-button");
                        _edit->setStyleClass("muda-done");
                        break;
                    case NOTE: 
                        _type->setText("note");
                        _type->setStyleClass("btn muda-note-button");
                        _edit->setStyleClass("muda-note");
                        break;
                }
            }
        }
    }
}
