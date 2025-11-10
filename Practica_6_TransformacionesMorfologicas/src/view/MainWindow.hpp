#pragma once
#include <gtkmm.h>
#include <memory>
#include "../model/ImageModel.hpp"

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow(std::shared_ptr<ImageModel> model);

    Gtk::Button& btn_load() { return btn_load_; }
    Gtk::Button& btn_save() { return btn_save_; }
    Gtk::CheckButton& chk_binarize(){ return chk_binarize_; }

    Gtk::ComboBoxText& cb_shape(){ return cb_shape_; }
    Gtk::SpinButton& sp_size(){ return sp_size_; }

    Gtk::Button& btn_erode(){ return btn_erode_; }
    Gtk::Button& btn_dilate(){ return btn_dilate_; }
    Gtk::Button& btn_open(){ return btn_open_; }
    Gtk::Button& btn_close(){ return btn_close_; }

    Gtk::Scale& scale_prob(){ return scale_prob_; }
    Gtk::Button& btn_add_salt(){ return btn_add_salt_; }
    Gtk::Button& btn_add_pepper(){ return btn_add_pepper_; }
    Gtk::Button& btn_remove_salt(){ return btn_remove_salt_; }
    Gtk::Button& btn_remove_pepper(){ return btn_remove_pepper_; }

    void refresh();

private:
    std::shared_ptr<ImageModel> model_;

    // Widgets
    Gtk::Box root_{Gtk::ORIENTATION_HORIZONTAL, 10};
    Gtk::Box left_{Gtk::ORIENTATION_VERTICAL, 8};
    Gtk::Separator sep_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box right_{Gtk::ORIENTATION_VERTICAL, 8};
    Gtk::Box top_imgs_{Gtk::ORIENTATION_HORIZONTAL, 8};

    Gtk::Frame fr_file_{"Archivo"};
    Gtk::Box bx_file_{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::Button btn_load_{"Cargar imagen"};
    Gtk::Button btn_save_{"Guardar resultado"};
    Gtk::CheckButton chk_binarize_{"Forzar binarización (umbral=128)"};

    Gtk::Frame fr_se_{"Elemento Estructurante"};
    Gtk::Box bx_se_{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::ComboBoxText cb_shape_;
    Glib::RefPtr<Gtk::Adjustment> adj_size_ = Gtk::Adjustment::create(3, 3, 31, 2, 2);
    Gtk::SpinButton sp_size_{adj_size_};

    Gtk::Frame fr_morph_{"Morfología"};
    Gtk::Box bx_morph_{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::Button btn_erode_{"Erosión"};
    Gtk::Button btn_dilate_{"Dilatación"};
    Gtk::Button btn_open_{"Apertura"};
    Gtk::Button btn_close_{"Clausura"};

    Gtk::Frame fr_noise_{"Ruido sal / pimienta"};
    Gtk::Box bx_noise_{Gtk::ORIENTATION_VERTICAL, 6};
    Glib::RefPtr<Gtk::Adjustment> adj_prob_ = Gtk::Adjustment::create(0.05, 0.0, 0.20, 0.01, 0.05);
    Gtk::Scale scale_prob_{adj_prob_};
    Gtk::Button btn_add_salt_{"Agregar sal"};
    Gtk::Button btn_add_pepper_{"Agregar pimienta"};
    Gtk::Button btn_remove_salt_{"Remover sal (Apertura)"};
    Gtk::Button btn_remove_pepper_{"Remover pimienta (Clausura)"};

    Gtk::Frame fr_in_{"Imagen"};
    Gtk::Frame fr_out_{"Resultado"};
    Gtk::Image img_in_w_;
    Gtk::Image img_out_w_;
};
