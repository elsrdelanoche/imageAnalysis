
#pragma once
#include <gtk/gtk.h>
#include <string>
#include "../core/image_buffer.hpp"
#include "view.hpp"

namespace ui {

class Controller {
    View& view_;
    core::ImageBuffer img_;
    core::ImageBuffer img_gray_;
public:
    Controller(View& v) : view_(v) {}
    void connect_signals();
    void show();
    void on_open_image();
    void on_apply_custom();
    void on_equalize();
    bool ensure_gray();
    void compute_and_draw_all();
};

} // namespace ui
