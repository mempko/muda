#ifndef MEMPKO_MUDA_WIDGET_H
#define MEMPKO_MUDA_WIDGET_H

#include <iostream>

#include <Wt/WCompositeWidget>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>

#include "core/muda.h"
#include "core/context.h"

namespace mempko { namespace muda { namespace wt { 

    class muda_widget : public Wt::WCompositeWidget
    {
        public:
            muda_widget(model::muda_ptr muda, Wt::WContainerWidget* parent = 0);
            ~muda_widget();
        private:
            void create_ui();
            void change_text();
            void update_text(const model::muda& muda);
        private:
            model::muda_ptr _muda;
            boost::signals2::connection _when_text_changes;
            Wt::WLineEdit * _edit;
            Wt::WContainerWidget* _layout;

    };

}}}//namespace

#endif
