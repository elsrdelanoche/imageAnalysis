#include <gtkmm/application.h>
#include "view/MainWindow.hpp"
#include "controller/AppController.hpp"
#include "model/ImageModel.hpp"

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.example.imlogic");
    auto model = std::make_shared<ImageModel>();
    MainWindow window(model);
    AppController controller(model, window);
    return app->run(window);
}
