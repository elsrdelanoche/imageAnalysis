#include <gtkmm/application.h>
#include "view/MainWindow.h"
#include "model/ImageModel.h"
#include "controller/TransformController.h"
#include <memory>
#include <iostream>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "com.imageprocessing.transform");
    
    // Crear modelo
    auto model = std::make_shared<ImageModel>();
    
    // Crear vista
    MainWindow window;
    
    // Crear controlador
    auto controller = std::make_shared<TransformController>(model, &window);
    
    // Conectar controlador con vista
    window.setController(controller);
    
    return app->run(window);
}
