#ifndef MEMPKO_MUDA_WIDGET_H
#define MEMPKO_MUDA_WIDGET_H

#include <iostream>
#include <list>

#include <Wt/WCompositeWidget>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WImage>
#include <Wt/WHBoxLayout>

#include "core/muda.h"
#include "core/context.h"

namespace mempko { namespace muda { namespace wt { 

    class muda_widget : public Wt::WCompositeWidget
    {
        public:
            muda_widget(model::muda_ptr muda, Wt::WContainerWidget* parent = 0);
            ~muda_widget();
        public:
            typedef boost::signals2::signal<void (id_type, muda_widget*)> delete_sig;
            typedef delete_sig::slot_type delete_slot;
            boost::signals2::connection when_delete_pressed(const delete_slot& slot);

        private:
            void create_ui();
            void change_text();
            void update_text();
            void show_delete();
            void hide_delete();
            void delete_pressed();
        private:
            model::muda_ptr _muda;
            boost::signals2::connection _when_text_changes;
            Wt::WContainerWidget* _root;
            Wt::WHBoxLayout* _layout;
            Wt::WLineEdit* _edit;
            Wt::WImage* _deleteButton;
            delete_sig _delete_sig;

    };

    typedef std::list<muda_widget*> muda_widget_list;

}}}//namespace

#endif
