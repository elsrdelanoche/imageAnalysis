#pragma once
#include <gtkmm.h>
#include <memory>
#include "../model/ImageModel.hpp"
class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow(std::shared_ptr<ImageModel> model);
    Gtk::Button& btn_load(){ return btn_load_; }
    Gtk::Button& btn_save(){ return btn_save_; }
    Gtk::Button& btn_apply(){ return btn_apply_; }
    Gtk::Button& btn_canny(){ return btn_canny_; }
    Gtk::ComboBoxText& cb_cat(){ return cb_cat_; }
    Gtk::ComboBoxText& cb_kernel(){ return cb_kernel_; }
    Gtk::SpinButton& sp_k(){ return sp_k_; }
    Gtk::SpinButton& sp_gauss_k(){ return sp_gauss_k_; }
    Gtk::SpinButton& sp_canny_k(){ return sp_canny_k_; }
    Gtk::SpinButton& sp_alpha(){ return sp_alpha_; }
    Gtk::SpinButton& sp_sigma(){ return sp_sigma_; }
    Gtk::SpinButton& sp_sigma_c(){ return sp_sigma_c_; }
    Gtk::SpinButton& sp_tlow(){ return sp_tlow_; }
    Gtk::SpinButton& sp_thigh(){ return sp_thigh_; }
    void refresh();
private:
    std::shared_ptr<ImageModel> model_;
    Gtk::Box root_{Gtk::ORIENTATION_HORIZONTAL, 10};
    Gtk::Box left_{Gtk::ORIENTATION_VERTICAL, 8};
    Gtk::Separator sep_{Gtk::ORIENTATION_VERTICAL};
    Gtk::Box right_{Gtk::ORIENTATION_VERTICAL, 8};
    Gtk::Box imgs_row_{Gtk::ORIENTATION_HORIZONTAL, 8};
    Gtk::Frame fr_file_{"Archivo"}; Gtk::Box bx_file_{Gtk::ORIENTATION_VERTICAL,6};
    Gtk::Button btn_load_{"Cargar imagen"}; Gtk::Button btn_save_{"Guardar resultado"};
    Gtk::Frame fr_cfg_{"Configuración"}; Gtk::Grid grid_;
    Gtk::Label lb_cat_{"Categoría:"}; Gtk::ComboBoxText cb_cat_;
    Gtk::Label lb_kernel_{"Kernel/Técnica:"}; Gtk::ComboBoxText cb_kernel_;
    Gtk::Label lb_k_{"k (impar):"}; Glib::RefPtr<Gtk::Adjustment> adj_k_ = Gtk::Adjustment::create(7,3,31,2,2); Gtk::SpinButton sp_k_{adj_k_};
    Gtk::Label lb_gauss_k_{"k Gauss:"}; Glib::RefPtr<Gtk::Adjustment> adj_gauss_k_ = Gtk::Adjustment::create(5,3,31,2,2); Gtk::SpinButton sp_gauss_k_{adj_gauss_k_};
    Gtk::Label lb_sigma_{"σ Gauss:"}; Glib::RefPtr<Gtk::Adjustment> adj_sigma_ = Gtk::Adjustment::create(1.0,0.2,5.0,0.1,0.5); Gtk::SpinButton sp_sigma_{adj_sigma_};
    Gtk::Label lb_alpha_{"α High-boost:"}; Glib::RefPtr<Gtk::Adjustment> adj_alpha_ = Gtk::Adjustment::create(1.0,0.0,5.0,0.1,0.5); Gtk::SpinButton sp_alpha_{adj_alpha_};
    Gtk::Label lb_canny_k_{"k Canny:"}; Glib::RefPtr<Gtk::Adjustment> adj_canny_k_ = Gtk::Adjustment::create(5,3,31,2,2); Gtk::SpinButton sp_canny_k_{adj_canny_k_};
    Gtk::Label lb_sigma_c_{"σ Canny:"}; Glib::RefPtr<Gtk::Adjustment> adj_sigma_c_ = Gtk::Adjustment::create(1.2,0.2,5.0,0.1,0.5); Gtk::SpinButton sp_sigma_c_{adj_sigma_c_};
    Gtk::Label lb_tlow_{"T_low:"}; Glib::RefPtr<Gtk::Adjustment> adj_tlow_ = Gtk::Adjustment::create(0.1,0.0,1.0,0.01,0.1); Gtk::SpinButton sp_tlow_{adj_tlow_};
    Gtk::Label lb_thigh_{"T_high:"}; Glib::RefPtr<Gtk::Adjustment> adj_thigh_ = Gtk::Adjustment::create(0.2,0.0,1.0,0.01,0.1); Gtk::SpinButton sp_thigh_{adj_thigh_};
    Gtk::Button btn_apply_{"Aplicar filtro"}; Gtk::Button btn_canny_{"Canny"};
    Gtk::Frame fr_in_{"Entrada"}; Gtk::Frame fr_out_{"Salida"};
    Gtk::Image img_in_w_, img_out_w_;
};
