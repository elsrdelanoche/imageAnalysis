#pragma once
#include "ui/view.hpp"
#include "ui/controller.hpp"

class App {
    ui::View       view_;
    ui::Controller controller_;
public:
    App() : controller_(view_) { controller_.connect_signals(); }
    void show() { controller_.show(); }
};
