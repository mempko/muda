#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

#include <list>
#include "core/muda.h"
#include "core/context.h"

using namespace Wt;

namespace mm = mempko::muda::model;
namespace mc = mempko::muda::context;

class app : public WApplication
{
    public:
        app(const WEnvironment& env);

    private:
        WLineEdit *_task;
        WText *_echo;

        void change_text();
        void update_text_display(const mm::muda& muda);
};

app::app(const WEnvironment& env) : WApplication(env)
{
    setTitle("Hello world");                               

    root()->addWidget(new WText("enter:")); 
    _task = new WLineEdit(root());                   
    _task->setFocus();                              

    WPushButton *b = new WPushButton("add", root()); 
    b->setMargin(5, Left);                                

    root()->addWidget(new WBreak());                     

    _echo = new WText(root());                      

    b->clicked().connect(this, &app::change_text);

    _task->enterPressed().connect
        (boost::bind(&app::change_text, this));
}

void app::update_text_display(const mm::muda& muda)
{
	_echo->setText(Wt::WString("change_text:") + muda.text());
    std::cout << "id: " << muda.id() << std::endl;
}

void app::change_text()
{
    mm::muda_list mudas;
    mm::muda_ptr muda(new mm::muda);

    mc::add_object<mm::muda,mm::muda_ptr,mm::muda_list> add(muda, mudas);
    add();
    std::cout << "total mudas: " << mudas.size() << std::endl;

	muda->when_text_changes(boost::bind(boost::mem_fn(&app::update_text_display), this, _1));

    mc::modify_text_context<mm::muda> change_muda_text(*muda, _task->text());

    change_muda_text();
}

WApplication *create_application(const WEnvironment& env)
{
    return new app(env);
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &create_application);
}

