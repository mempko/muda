
#include "wtui/mudawidget.h"

namespace mempko { namespace muda { namespace wt { 

    using boost::bind;
    using boost::mem_fn;
    namespace w = Wt;

    muda_widget::muda_widget(model::muda_ptr muda, w::WContainerWidget* parent) :
        w::WCompositeWidget(parent),
        _muda(muda)
    {
        BOOST_ASSERT(_muda);
        setImplementation(_layout = new w::WContainerWidget);
        create_ui();
    }

    muda_widget::~muda_widget()
    {
        _when_text_changes.disconnect();
    }

    void muda_widget::create_ui()
    {
        BOOST_ASSERT(_muda);
        //create text editor
        _edit = new w::WLineEdit(_layout);
        _edit->resize(w::WLength(99, w::WLength::Percentage), w::WLength::Auto);
        _edit->setStyleClass("muda");
        //set text to muda text
        _edit->setText(_muda->text());

        //when enter is pressed change the model
        _edit->enterPressed().connect
            (bind(&muda_widget::change_text, this));

        //when the model changes update the text
        _when_text_changes = _muda->when_text_changes(
                bind(&muda_widget::update_text, this));
    }

    void muda_widget::change_text()
    {
        BOOST_ASSERT(_muda);
        context::modify_muda_text modify_text(*_muda, _edit->text());
        modify_text();
    }

    void muda_widget::update_text()
    {
        BOOST_ASSERT(_muda);
        _edit->setText(w::WString("changed_text: ") + _muda->text());
        std::cout << "id: " << _muda->id() << std::endl;
    }

}}}//namespace
