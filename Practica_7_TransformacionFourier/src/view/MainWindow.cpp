#include "MainWindow.hpp"

MainWindow::MainWindow(std::shared_ptr<ImageModel> model) : model_(std::move(model)){
    set_title("Fourier 2D (GTK3 · C++)");
    set_default_size(1200, 760);

    add(root_);
    root_.pack_start(left_, Gtk::PACK_SHRINK);
    root_.pack_start(sep_, Gtk::PACK_SHRINK);
    root_.pack_start(right_, Gtk::PACK_EXPAND_WIDGET);

    // Left controls
    fr_file_.add(bx_file_);
    bx_file_.pack_start(btn_load_, Gtk::PACK_SHRINK);
    bx_file_.pack_start(btn_save_, Gtk::PACK_SHRINK);

    fr_fft_.add(bx_fft_);
    bx_fft_.pack_start(btn_fft_, Gtk::PACK_SHRINK);
    chk_shift_.set_active(true);
    bx_fft_.pack_start(chk_shift_, Gtk::PACK_SHRINK);
    bx_fft_.pack_start(btn_ifft_, Gtk::PACK_SHRINK);

    left_.pack_start(fr_file_, Gtk::PACK_SHRINK);
    left_.pack_start(fr_fft_, Gtk::PACK_SHRINK);

    // Right images
    fr_in_.add(img_in_w_);
    fr_mag_.add(img_mag_w_);
    fr_phase_.add(img_phase_w_);
    fr_out_.add(img_out_w_);

    top_imgs_.pack_start(fr_in_, Gtk::PACK_EXPAND_WIDGET);
    top_imgs_.pack_start(fr_mag_, Gtk::PACK_EXPAND_WIDGET);
    top_imgs_.pack_start(fr_phase_, Gtk::PACK_EXPAND_WIDGET);

    bottom_imgs_.pack_start(fr_out_, Gtk::PACK_EXPAND_WIDGET);

    right_.pack_start(top_imgs_, Gtk::PACK_EXPAND_WIDGET);
    right_.pack_start(bottom_imgs_, Gtk::PACK_EXPAND_WIDGET);

    show_all_children();
}

void MainWindow::refresh(){
    auto A = model_->img_in();
    if(A) img_in_w_.set(A); else img_in_w_.clear();

    auto M = model_->img_mag();
    if(M) img_mag_w_.set(M); else img_mag_w_.clear();

    auto P = model_->img_phase();
    if(P) img_phase_w_.set(P); else img_phase_w_.clear();

    auto R = model_->img_result();
    if(R) img_out_w_.set(R); else img_out_w_.clear();
}
