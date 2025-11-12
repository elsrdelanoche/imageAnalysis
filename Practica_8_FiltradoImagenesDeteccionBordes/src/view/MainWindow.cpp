#include "MainWindow.hpp"
MainWindow::MainWindow(std::shared_ptr<ImageModel> model) : model_(std::move(model)){
    set_title("Filtros y Bordes (GTK3 · C++)"); set_default_size(1200, 760);
    add(root_); root_.pack_start(left_, Gtk::PACK_SHRINK); root_.pack_start(sep_, Gtk::PACK_SHRINK); root_.pack_start(right_, Gtk::PACK_EXPAND_WIDGET);
    fr_file_.add(bx_file_); bx_file_.pack_start(btn_load_, Gtk::PACK_SHRINK); bx_file_.pack_start(btn_save_, Gtk::PACK_SHRINK);
    fr_cfg_.add(grid_); int r=0;
    grid_.attach(lb_cat_,0,r,1,1); grid_.attach(cb_cat_,1,r,2,1); ++r;
    grid_.attach(lb_kernel_,0,r,1,1); grid_.attach(cb_kernel_,1,r,2,1); ++r;
    grid_.attach(lb_k_,0,r,1,1); grid_.attach(sp_k_,1,r,2,1); ++r;
    grid_.attach(lb_gauss_k_,0,r,1,1); grid_.attach(sp_gauss_k_,1,r,2,1); ++r;
    grid_.attach(lb_sigma_,0,r,1,1); grid_.attach(sp_sigma_,1,r,2,1); ++r;
    grid_.attach(lb_alpha_,0,r,1,1); grid_.attach(sp_alpha_,1,r,2,1); ++r;
    grid_.attach(lb_canny_k_,0,r,1,1); grid_.attach(sp_canny_k_,1,r,2,1); ++r;
    grid_.attach(lb_sigma_c_,0,r,1,1); grid_.attach(sp_sigma_c_,1,r,2,1); ++r;
    grid_.attach(lb_tlow_,0,r,1,1); grid_.attach(sp_tlow_,1,r,2,1); ++r;
    grid_.attach(lb_thigh_,0,r,1,1); grid_.attach(sp_thigh_,1,r,2,1); ++r;
    grid_.attach(btn_apply_,0,r,2,1); grid_.attach(btn_canny_,2,r,1,1); ++r;
    cb_cat_.append("Pasa-bajas"); cb_cat_.append("Pasa-altas"); cb_cat_.append("Bordes"); cb_cat_.set_active(0);
    const char* items[] = {"Media 3x3","Media 5x5","Media 7x7","Gauss","Sharpen","Laplaciano4","Laplaciano8","High-boost",
        "Roberts Gx","Roberts Gy","Prewitt Gx","Prewitt Gy","Prewitt (mag)","Sobel Gx","Sobel Gy","Sobel (mag)",
        "Scharr Gx","Scharr Gy","Kirsch N","Kirsch E","Robinson N","Robinson E","LoG"};
    for(auto s: items) cb_kernel_.append(s); cb_kernel_.set_active(0);
    left_.pack_start(fr_file_, Gtk::PACK_SHRINK); left_.pack_start(fr_cfg_, Gtk::PACK_SHRINK);
    fr_in_.add(img_in_w_); fr_out_.add(img_out_w_);
    imgs_row_.pack_start(fr_in_, Gtk::PACK_EXPAND_WIDGET); imgs_row_.pack_start(fr_out_, Gtk::PACK_EXPAND_WIDGET);
    right_.pack_start(imgs_row_, Gtk::PACK_EXPAND_WIDGET);
    show_all_children();
}
void MainWindow::refresh(){
    auto A=model_->img_in(); if(A) img_in_w_.set(A); else img_in_w_.clear();
    auto R=model_->img_out(); if(R) img_out_w_.set(R); else img_out_w_.clear();
}
