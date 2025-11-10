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

    void on_load();
    void on_save();

    void on_change_se();
    void on_erode();
    void on_dilate();
    void on_open();
    void on_close();

    void on_add_salt();
    void on_add_pepper();
    void on_remove_salt();
    void on_remove_pepper();

    double current_prob() const;
};
