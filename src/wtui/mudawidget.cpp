
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
        setImplementation(_root = new w::WContainerWidget);
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
        _edit = new w::WLineEdit();
        _edit->resize(w::WLength(100, w::WLength::Percentage), w::WLength::Auto);
        _edit->setStyleClass("muda");
        //set text to muda text
        _edit->setText(_muda->text());

        //when enter is pressed change the model
        _edit->enterPressed().connect
            (bind(&muda_widget::change_text, this));

        //when the model changes update the text
        _when_text_changes = _muda->when_text_changes(
                bind(&muda_widget::update_text, this));

        //delete button
        _deleteButton = new w::WImage("images/delete.png");
        _deleteButton->setPopup(true);
        _deleteButton->setStyleClass("muda-delete");

        hide_delete();

        _deleteButton->clicked().connect
            (bind(&muda_widget::delete_pressed, this));

        //layout
        _layout = new w::WHBoxLayout();
        _layout->setSpacing(0);
        _layout->addWidget(_edit);
        _layout->addWidget(_deleteButton);
        _layout->setStretchFactor(_edit, 1);
        _layout->setContentsMargins(0,0,0,0);

        _root->setLayout(_layout);
        _root->setStyleClass("muda-container");
        _root->resize(w::WLength(99, w::WLength::Percentage), w::WLength::Auto);

        _root->mouseWentOver().connect
            (bind(&muda_widget::show_delete, this));
        _root->mouseWentOut().connect
            (bind(&muda_widget::hide_delete, this));
    }

    void muda_widget::change_text()
    {
        BOOST_ASSERT(_muda);
        BOOST_ASSERT(_edit);
        context::modify_muda_text modify_text(*_muda, _edit->text());
        modify_text();
    }

    void muda_widget::update_text()
    {
        BOOST_ASSERT(_muda);
        _edit->setText(_muda->text());
        std::cout << "id: " << _muda->id() << std::endl;
    }

    void muda_widget::show_delete()
    {
        BOOST_ASSERT(_deleteButton);
        _deleteButton->setHidden(false);
    }

    void muda_widget::hide_delete()
    {
        BOOST_ASSERT(_deleteButton);
        _deleteButton->setHidden(true);
    }

    void muda_widget::delete_pressed()
    {
        BOOST_ASSERT(_muda);
        hide_delete();
        _delete_sig(_muda->id(), this);
    }

    boost::signals2::connection muda_widget::when_delete_pressed(const delete_slot& slot) 
    { 
        return _delete_sig.connect(slot);
    }

}}}//namespace
