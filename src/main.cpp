#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

#include <list>
#include "core/muda.h"
#include "core/context.h"
#include "wtui/mudawidget.h"

using namespace Wt;

namespace mm = mempko::muda::model;
namespace mc = mempko::muda::context;
namespace mt = mempko::muda::wt;

class app : public WApplication
{
    public:
        app(const WEnvironment& env);
    private:
        void create_ui();
    private:
        mt::muda_widget* _muda_widget;
        mm::muda_list _mudas;
};

app::app(const WEnvironment& env) : WApplication(env)
{
    setTitle("Hello world");                               
    create_ui();
}

void app::create_ui()
{
    mm::muda_ptr muda(new mm::muda);

    //add muda to muda list
    mc::add_muda add(muda, _mudas);
    add();

    _muda_widget = new mt::muda_widget(muda, root());

    std::cout << "total mudas: " << _mudas.size() << std::endl;
}

WApplication *create_application(const WEnvironment& env)
{
    return new app(env);
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &create_application);
}

