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

#include "wtui/mudalistwidget.h"

namespace mempko { namespace muda { namespace wt { 

    using boost::bind;
    using boost::mem_fn;
    namespace w = Wt;

    muda_list_widget::muda_list_widget(model::muda_list& mudas, muda_list_widget::filter_func filter, w::WContainerWidget* parent) :
        w::WCompositeWidget(parent),
        _mudas(mudas), _filter(filter)
    {
        setImplementation(_root = new w::WContainerWidget);
        create_ui();
    }

    void muda_list_widget::create_ui()
    {
        _root->resize(w::WLength(100, w::WLength::Percentage), w::WLength::Auto);

        std::for_each(_mudas.begin(), _mudas.end(),
            bind(mem_fn(&muda_list_widget::add_muda), this, _1));
    }

    void muda_list_widget::update()
    {
        _root->clear();
        create_ui();
    }

    void muda_list_widget::add_muda(model::muda_ptr muda)
    {
        BOOST_ASSERT(muda);

        //do not add if it does not pass filter
        if(!_filter(muda)) return;

        muda->when_text_changes(bind(&muda_list_widget::fire_update_sig, this));

        //create new muda widget and add it to widget list
        muda_widget* new_widget = new muda_widget(muda);

        new_widget->when_delete_pressed(bind(&muda_list_widget::remove_muda, this, _1, _2));
        new_widget->when_type_pressed(bind(&muda_list_widget::fire_update_sig, this));

        if(_muda_widgets.empty()) _root->addWidget(new_widget);
        else _root->insertBefore(new_widget, _muda_widgets.back());

        _muda_widgets.push_back(new_widget);
    }

    void muda_list_widget::remove_muda(id_type id, muda_widget* widget)
    {
        context::remove_muda remove(id, _mudas); remove();

        _muda_widgets.remove(widget);
        _root->removeWidget(widget);
        fire_update_sig();
    }

    void muda_list_widget::fire_update_sig()
    {
        _update_sig();
    }

    boost::signals2::connection muda_list_widget::when_model_updated(const update_slot& slot)
    {
        return _update_sig.connect(slot);
    }

}}}//namespace
