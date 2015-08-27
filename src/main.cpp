#include <fstream>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLabel>

#include "core/muda.h"
#include "core/context.h"
#include "wtui/mudalistwidget.h"

using namespace Wt;
using boost::bind;
using boost::mem_fn;

namespace m = mempko::muda;
namespace mm = mempko::muda::model;
namespace mc = mempko::muda::context;
namespace mt = mempko::muda::wt;

class app : public WApplication
{
    public:
        app(const WEnvironment& env);
    private:
        void login_screen();
        void muda_screen();
        void create_mudas_ui();
        void add_new_muda();
        void save_mudas();
        void load_mudas();
        bool filter();
    private:
        //login widgets
        WLineEdit* _muda_file_edit;
        std::string _muda_file;
    private:
        //muda widgets
        WLineEdit* _new_muda;
        mm::muda_list _mudas;
        mt::muda_list_widget* _muda_list_widget;
};

app::app(const WEnvironment& env) : 
    WApplication(env),
    _muda_file("global")
{
    login_screen();
}

void app::login_screen()
{
    root()->clear();
    WHBoxLayout* layout = new WHBoxLayout;

    layout->addWidget(new WLabel("Muda List:"), 0, AlignMiddle);
    layout->addWidget( _muda_file_edit = new WLineEdit(_muda_file), 0, AlignMiddle);
    _muda_file_edit->setFocus();
    _muda_file_edit->setStyleClass("muda");

    WPushButton* b = new WPushButton("Load");
    layout->addWidget(b, 0, AlignMiddle);
    layout->addStretch();

    b->clicked().connect(bind(&app::muda_screen, this));
    _muda_file_edit->enterPressed().connect(bind(&app::muda_screen, this));
    root()->setLayout(layout);
}

void app::muda_screen()
{
    _muda_file = _muda_file_edit->text().narrow();
    setTitle(_muda_file);
    root()->clear();
    load_mudas();
    create_mudas_ui();
}

void app::save_mudas()
{
    std::ofstream o(_muda_file.c_str());
    boost::archive::xml_oarchive oa(o);

    oa << BOOST_SERIALIZATION_NVP(_mudas);
}
void app::load_mudas()
{
    std::ifstream i(_muda_file.c_str());
    if(!i) return;

    boost::archive::xml_iarchive ia(i);

    ia >> BOOST_SERIALIZATION_NVP(_mudas);
}

void app::create_mudas_ui()
{
    _new_muda = new WLineEdit(root());
    _new_muda->resize(WLength(99, WLength::Percentage), WLength::Auto);
    _new_muda->setStyleClass("new-muda");

    //add muda when enter is pressed
    _new_muda->enterPressed().connect
        (bind(&app::add_new_muda, this));

    //create muda list widget
    _muda_list_widget = new mt::muda_list_widget(_mudas, bind(&app::filter, this), root());
    _muda_list_widget->when_model_updated(bind(&app::save_mudas, this));
}

bool app::filter()
{
    return true;
}

void app::add_new_muda()
{
    BOOST_ASSERT(_new_muda);

    //create new muda and add it to muda list
    mm::muda_ptr muda(new mm::muda);

    mc::modify_muda_text modify_text(*muda, _new_muda->text());
    modify_text();

    mc::add_muda add(muda, _mudas);
    add();

    _muda_list_widget->add_muda(muda);
    _new_muda->setText(L"");

    std::cout << "total mudas: " << _mudas.size() << std::endl;
    save_mudas();
}

WApplication *create_application(const WEnvironment& env)
{
    WApplication* a = new app(env);
    a->useStyleSheet("style.css");
    a->setTitle("Muda");                               
    return a;
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &create_application);
}

