#pragma once
#include <gtkmm.h>
#include <memory>
#include "../model/ImageModel.hpp"

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow(std::shared_ptr<ImageModel> model);

    // view accessors for controller
    Gtk::Button& btn_load_a() { return *btn_load_a_; }
    Gtk::Button& btn_load_b() { return *btn_load_b_; }
    Gtk::Button& btn_apply()  { return *btn_apply_; }
    Gtk::Button& btn_save()   { return *btn_save_; }
    Gtk::CheckButton& chk_binarize() { return *chk_binarize_; }
    Gtk::CheckButton& chk_not_a() { return *chk_not_a_; }
    Gtk::CheckButton& chk_not_b() { return *chk_not_b_; }

    // radio getters
    Gtk::RadioButton& rb_and() { return *rb_and_; }
    Gtk::RadioButton& rb_or()  { return *rb_or_; }
    Gtk::RadioButton& rb_xor() { return *rb_xor_; }

    Gtk::RadioButton& rb_eq() { return *rb_eq_; }
    Gtk::RadioButton& rb_ne() { return *rb_ne_; }
    Gtk::RadioButton& rb_gt() { return *rb_gt_; }
    Gtk::RadioButton& rb_ge() { return *rb_ge_; }
    Gtk::RadioButton& rb_lt() { return *rb_lt_; }
    Gtk::RadioButton& rb_le() { return *rb_le_; }

    void refresh_images();

private:
    std::shared_ptr<ImageModel> model_;
    Glib::RefPtr<Gtk::Builder> builder_;

    // widgets
    Gtk::Button* btn_load_a_ = nullptr;
    Gtk::Button* btn_load_b_ = nullptr;
    Gtk::Button* btn_apply_  = nullptr;
    Gtk::Button* btn_save_   = nullptr;

    Gtk::CheckButton* chk_binarize_ = nullptr;
    Gtk::CheckButton* chk_not_a_ = nullptr;
    Gtk::CheckButton* chk_not_b_ = nullptr;

    Gtk::Image* img_a_ = nullptr;
    Gtk::Image* img_b_ = nullptr;
    Gtk::Image* img_result_ = nullptr;

    Gtk::RadioButton* rb_and_ = nullptr;
    Gtk::RadioButton* rb_or_  = nullptr;
    Gtk::RadioButton* rb_xor_ = nullptr;
    Gtk::RadioButton* rb_eq_ = nullptr;
    Gtk::RadioButton* rb_ne_ = nullptr;
    Gtk::RadioButton* rb_gt_ = nullptr;
    Gtk::RadioButton* rb_ge_ = nullptr;
    Gtk::RadioButton* rb_lt_ = nullptr;
    Gtk::RadioButton* rb_le_ = nullptr;
};
