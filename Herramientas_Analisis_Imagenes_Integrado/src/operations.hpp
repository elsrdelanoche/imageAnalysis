#pragma once
#include <gtkmm.h>
#include "CanvasArea.hpp"
#include "LeftPanel.hpp"
#include "utils/ImgUtils.hpp"
#include "utils/Convolution.hpp"
#include "utils/Kernels.hpp"
#include "utils/Canny.hpp"

class AppController;

namespace Operations {

// Filtros lineales / Canny (Práctica 8)
void apply_lowpass(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void apply_highpass(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void apply_edge(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void apply_canny(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);

// Color / histogramas / lógicas / geométricas / morfología / Fourier
void do_color_transforms(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void do_histogram(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void do_logical_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void do_geom_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void do_morph_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);
void do_fourier_ops(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel);

// Placeholder genérico
void not_implemented(Gtk::Window& win, const Glib::ustring& name);

} // namespace Operations
