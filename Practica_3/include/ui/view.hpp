
#pragma once
#include <gtk/gtk.h>
#include <string>
#include "../core/image_buffer.hpp"

namespace ui {

class View {
    GtkWidget *window_;
    GtkWidget *left_panel_;
    GtkWidget *right_panel_;
    GtkWidget *grid_right_;

    GtkWidget *btn_open_;
    GtkWidget *img_input_;
    GtkWidget *cmb_transform_;
    GtkWidget *scale_gamma_;
    GtkWidget *btn_apply_;
    GtkWidget *btn_equalize_;

    GtkWidget *img_hist_;
    GtkWidget *img_pdf_;
    GtkWidget *img_cdf_;
    GtkWidget *stats_view_;
    GtkWidget *img_equalized_;
    GtkWidget *img_pdf_eq_;
    GtkWidget *img_custom_;
    GtkWidget *img_hist_custom_;
public:
    static constexpr int TILE_W = 560;
    static constexpr int TILE_H = 320;
    static constexpr int LEFT_IMG_W = 300;
    static constexpr int LEFT_IMG_H = 220;

    View();
    void build_ui();
    void show() { gtk_widget_show_all(window_); }

    GtkWidget* window() const { return window_; }
    GtkWidget* btn_open() const { return btn_open_; }
    GtkWidget* btn_apply() const { return btn_apply_; }
    GtkWidget* btn_equalize() const { return btn_equalize_; }
    GtkWidget* img_input() const { return img_input_; }
    GtkWidget* cmb_transform() const { return cmb_transform_; }
    GtkWidget* scale_gamma() const { return scale_gamma_; }

    void set_hist_pixbuf(GdkPixbuf* px);
    void set_pdf_pixbuf(GdkPixbuf* px);
    void set_cdf_pixbuf(GdkPixbuf* px);
    void set_equalized_image(GdkPixbuf* px);
    void set_pdf_eq_pixbuf(GdkPixbuf* px);
    void set_custom_image(GdkPixbuf* px);
    void set_custom_hist(GdkPixbuf* px);
    void set_stats_text(const std::string& t);
    void set_input_image(GdkPixbuf* px);
};

} // namespace ui
