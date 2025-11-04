#pragma once
#include <gtkmm.h>
#include <memory>
#include "../model/ImageModel.hpp"
#include "../view/MainWindow.hpp"

class AppController {
public:
    AppController(std::shared_ptr<ImageModel> model, MainWindow& view);

private:
    std::shared_ptr<ImageModel> model_;
    MainWindow& view_;

    void on_load_a();
    void on_load_b();
    void on_apply();
    void on_save();

    void show_error(const std::string& msg);
};
