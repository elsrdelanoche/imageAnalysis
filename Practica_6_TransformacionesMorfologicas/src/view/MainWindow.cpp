#include "MainWindow.hpp"

MainWindow::MainWindow(std::shared_ptr<ImageModel> model) : model_(std::move(model)){
    set_title("Morfología (GTK3 · C++)");
    set_default_size(1100, 680);

    add(root_);
    root_.pack_start(left_, Gtk::PACK_SHRINK);
    root_.pack_start(sep_, Gtk::PACK_SHRINK);
    root_.pack_start(right_, Gtk::PACK_EXPAND_WIDGET);

    // ----- File frame -----
    fr_file_.add(bx_file_);
    bx_file_.pack_start(btn_load_, Gtk::PACK_SHRINK);
    bx_file_.pack_start(btn_save_, Gtk::PACK_SHRINK);
    chk_binarize_.set_active(true);
    bx_file_.pack_start(chk_binarize_, Gtk::PACK_SHRINK);

    // ----- SE frame -----
    fr_se_.add(bx_se_);
    cb_shape_.append("Cuadrado");
    cb_shape_.append("Diamante");
    cb_shape_.append("Disco");
    cb_shape_.set_active(0);
    bx_se_.pack_start(cb_shape_, Gtk::PACK_SHRINK);
    sp_size_.set_digits(0);
    bx_se_.pack_start(sp_size_, Gtk::PACK_SHRINK);

    // ----- Morph frame -----
    fr_morph_.add(bx_morph_);
    bx_morph_.pack_start(btn_erode_, Gtk::PACK_SHRINK);
    bx_morph_.pack_start(btn_dilate_, Gtk::PACK_SHRINK);
    bx_morph_.pack_start(btn_open_, Gtk::PACK_SHRINK);
    bx_morph_.pack_start(btn_close_, Gtk::PACK_SHRINK);

    // ----- Noise frame -----
    fr_noise_.add(bx_noise_);
    scale_prob_.set_digits(2);
    scale_prob_.set_draw_value(true);
    bx_noise_.pack_start(scale_prob_, Gtk::PACK_SHRINK);
    bx_noise_.pack_start(btn_add_salt_, Gtk::PACK_SHRINK);
    bx_noise_.pack_start(btn_add_pepper_, Gtk::PACK_SHRINK);
    bx_noise_.pack_start(btn_remove_salt_, Gtk::PACK_SHRINK);
    bx_noise_.pack_start(btn_remove_pepper_, Gtk::PACK_SHRINK);

    // Left panel compose
    left_.pack_start(fr_file_, Gtk::PACK_SHRINK);
    left_.pack_start(fr_se_, Gtk::PACK_SHRINK);
    left_.pack_start(fr_morph_, Gtk::PACK_SHRINK);
    left_.pack_start(fr_noise_, Gtk::PACK_SHRINK);

    // Right panel images
    fr_in_.add(img_in_w_);
    fr_out_.add(img_out_w_);
    top_imgs_.pack_start(fr_in_, Gtk::PACK_EXPAND_WIDGET);
    top_imgs_.pack_start(fr_out_, Gtk::PACK_EXPAND_WIDGET);
    right_.pack_start(top_imgs_, Gtk::PACK_EXPAND_WIDGET);

    show_all_children();
}

void MainWindow::refresh(){
    auto A = model_->img_in();
    if(A) img_in_w_.set(A);
    else  img_in_w_.clear();
    auto R = model_->img_out();
    if(R) img_out_w_.set(R);
    else  img_out_w_.clear();
}
