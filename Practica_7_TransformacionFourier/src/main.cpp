#include <gtkmm/application.h>
#include "view/MainWindow.hpp"
#include "model/ImageModel.hpp"
#include "controller/AppController.hpp"

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.example.fourier");
    auto model = std::make_shared<ImageModel>();
    MainWindow win(model);
    AppController controller(model, win);
    return app->run(win);
}
