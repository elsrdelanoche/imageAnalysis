#pragma once
#include <gtkmm.h>
#include <memory>
#include "../model/ImageModel.hpp"
#include "../view/MainWindow.hpp"
class AppController {
public:
    AppController(std::shared_ptr<ImageModel> model, MainWindow& view);
private:
    std::shared_ptr<ImageModel> model_; MainWindow& view_;
    void on_load(); void on_save(); void on_apply(); void on_canny();

    void on_cat_changed();
    void on_kernel_changed();
    void refresh_kernel_list();
    void refresh_param_sensitivity();
};
