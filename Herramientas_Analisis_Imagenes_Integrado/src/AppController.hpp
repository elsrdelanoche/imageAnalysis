#pragma once
#include <gtkmm.h>
#include "CanvasArea.hpp"
#include "LeftPanel.hpp"

class AppController {
public:
    AppController(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);

    // Archivo
    void on_open_image();
    void on_quit();

    // Grupos de herramientas
    void on_color_transforms();
    void on_histograma();
    void on_logical_ops();
    void on_geom_ops();
    void on_morph_ops();
    void on_fourier_ops();

    // Filtros lineales / Canny
    void on_lowpass();
    void on_highpass();
    void on_edge();
    void on_canny();

    // Ayuda
    void on_about();

private:
    Gtk::Window& win_;
    CanvasArea& canvas_;
    LeftPanel& panel_;

    void show_info(const Glib::ustring& msg);
};
