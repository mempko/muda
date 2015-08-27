
#include "wtui/mudawidget.h"

namespace mempko { namespace muda { namespace wt { 

    using boost::bind;
    using boost::mem_fn;

    muda_widget::muda_widget(model::muda_ptr muda, Wt::WContainerWidget* parent) :
        Wt::WCompositeWidget(parent),
        _muda(muda)
    {
        setImplementation(_layout = new Wt::WContainerWidget);
        create_ui();
    }

    muda_widget::~muda_widget()
    {
        _when_text_changes.disconnect();
    }

    void muda_widget::create_ui()
    {
        _edit = new Wt::WLineEdit(_layout);
        //when enter is pressed change the model
        _edit->enterPressed().connect
            (bind(&muda_widget::change_text, this));

        //when the model changes update the text
        _when_text_changes = _muda->when_text_changes(
                bind(mem_fn(&muda_widget::update_text), this, _1));
    }

    void muda_widget::change_text()
    {
        context::modify_text_context<model::muda> change_muda_text(*_muda, _edit->text());
        change_muda_text();
    }

    void muda_widget::update_text(const model::muda& muda)
    {
        _edit->setText(Wt::WString("change_text:") + muda.text());
        std::cout << "id: " << muda.id() << std::endl;
    }

}}}//namespace
