#pragma once
#include "view.hpp"
#include "../core/pipeline.hpp"
#include "../ops/grayscale.hpp"
#include "../ops/brightness_contrast.hpp"
#include "../ops/channel_extractor.hpp"
#include "../ops/resize.hpp"
#include "../model/adapters.hpp"
#include "../io/exporter.hpp"
#include "../io/loader.hpp"

namespace ui {

// Controlador: orquesta modelo (pipelines) y vista GTK
class Controller {
public:
    Controller(View& v) : view_(v) {}
    ~Controller();

    void connect_signals();
    void show() { view_.show(); }

private:
    // señales
    static void on_load(GtkWidget*, gpointer self);
    static void on_export(GtkWidget*, gpointer self);
    static void on_brightness(GtkRange*, gpointer self);
    static void on_contrast(GtkRange*, gpointer self);

    // flujo
    void load_dialog();
    void process_loaded(const char* path);
    void refresh_color();
    void refresh_gray();
    void export_dialog();

    // util
    io::IntImageMode mode_from_combo() const;

private:
    View& view_;
    // estado imagen/buffers base
    core::ImageBuffer base_color_; // escalada
    core::ImageBuffer base_gray_;  // gris base
    // retain original path if needed
    double brightness_ = 0.0;
    double contrast_ = 1.0;
};

} // namespace ui
