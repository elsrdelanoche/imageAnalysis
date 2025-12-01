#include <gtkmm.h>
#include "AppWindow.hpp"

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "mx.unam.herramientas.analisis.imagenes");
    AppWindow win;
    return app->run(win);
}
