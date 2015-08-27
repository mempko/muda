#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

using namespace Wt;

class App : public WApplication
{
    public:
        App(const WEnvironment& env);

    private:
        WLineEdit *_task;
        WText *_echo;

        void echo();
};

App::App(const WEnvironment& env) : WApplication(env)
{
    setTitle("Hello world");                               

    root()->addWidget(new WText("enter:")); 
    _task = new WLineEdit(root());                   
    _task->setFocus();                              

    WPushButton *b = new WPushButton("add", root()); 
    b->setMargin(5, Left);                                

    root()->addWidget(new WBreak());                     

    _echo = new WText(root());                      

    b->clicked().connect(this, &App::echo);

    _task->enterPressed().connect
        (boost::bind(&App::echo, this));
}

void App::echo()
{
    _echo->setText("echo:" + _task->text());
}

WApplication *createApplication(const WEnvironment& env)
{
    return new App(env);
}

int main(int argc, char **argv)
{
    return WRun(argc, argv, &createApplication);
}

