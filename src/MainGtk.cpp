#include "utils/Log.h"

#include <L4/web/Http.h>
#include <gtkmm.h>

class MyWindow : public Gtk::Window
{
public:
    MyWindow();
};

MyWindow::MyWindow()
{
    set_title("Basic application");
    set_default_size(200, 200);
}

namespace L4
{
    void Main()
    {
        LogSetup();
        auto app = Gtk::Application::create("org.gtkmm.examples.base");

        app->make_window_and_run<MyWindow>(argc, argv);
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}