#pragma once
#include <gtkmm.h>
#include <memory>
#include "../model/ImageModel.hpp"

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow(std::shared_ptr<ImageModel> model);

    Gtk::Button& btn_load() { return btn_load_; }
    Gtk::Button& btn_fft() { return btn_fft_; }
    Gtk::Button& btn_ifft() { return btn_ifft_; }
    Gtk::Button& btn_save() { return btn_save_; }
    Gtk::CheckButton& chk_shift(){ return chk_shift_; }

    void refresh();

private:
    std::shared_ptr<ImageModel> model_;

    Gtk::Box root_{Gtk::ORIENTATION_HORIZONTAL, 10};
    Gtk::Box left_{Gtk::ORIENTATION_VERTICAL, 8};
    Gtk::Separator sep_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box right_{Gtk::ORIENTATION_VERTICAL, 8};
    Gtk::Box top_imgs_{Gtk::ORIENTATION_HORIZONTAL, 8};
    Gtk::Box bottom_imgs_{Gtk::ORIENTATION_HORIZONTAL, 8};

    Gtk::Frame fr_file_{"Archivo"};
    Gtk::Box bx_file_{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::Button btn_load_{"Cargar imagen"};
    Gtk::Button btn_save_{"Guardar resultado"};

    Gtk::Frame fr_fft_{"Fourier"};
    Gtk::Box bx_fft_{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::Button btn_fft_{"FFT (→ Mag/Fase)"};
    Gtk::CheckButton chk_shift_{"Shift (centrar bajas frecuencias)"};
    Gtk::Button btn_ifft_{"IFFT (reconstruir)"};

    Gtk::Frame fr_in_{"Imagen N×N (pow2)"};
    Gtk::Frame fr_mag_{"Magnitud (log)"};
    Gtk::Frame fr_phase_{"Fase"};
    Gtk::Frame fr_out_{"Reconstrucción"};
    Gtk::Image img_in_w_, img_mag_w_, img_phase_w_, img_out_w_;
};
