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

    class muda_list_widget : public Wt::WCompositeWidget
    {
        public:
            typedef boost::function<bool (model::muda_ptr)> filter_func;
            muda_list_widget(
                    model::muda_list& mudas, 
                    filter_func filter, 
                    Wt::WContainerWidget* parent = 0);
        public:
            typedef boost::signals2::connection connection;
            typedef boost::signals2::signal<void ()> update_sig;
            typedef update_sig::slot_type update_slot;
            connection when_model_updated(const update_slot& slot);
        public:
            void add_muda(model::muda_ptr muda);
            void update();
        private:
            void create_ui();
            void remove_muda(id_type id, muda_widget* widget);
            void fire_update_sig();
        private:
            filter_func _filter;
            model::muda_list& _mudas;
            muda_widget_list _muda_widgets;
            update_sig _update_sig;
            Wt::WContainerWidget* _root;
    };

}}}//namespace
#endif
