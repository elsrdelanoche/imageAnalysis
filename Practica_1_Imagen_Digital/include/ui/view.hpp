#pragma once
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

namespace ui {

// Vista GTK pura (no procesa imagen). Expone widgets clave.
class View {
public:
    View();
    ~View() = default;

    GtkWidget* window() const { return window_; }
    GtkWidget* btn_load() const { return btn_load_; }
    GtkWidget* btn_export() const { return btn_export_; }
    GtkWidget* scale_brightness() const { return scale_brightness_; }
    GtkWidget* scale_contrast() const { return scale_contrast_; }
    GtkWidget* combo_mode() const { return combo_mode_; }

    void set_filename(const char* name);
    void set_original(GdkPixbuf* px);
    void set_gray(GdkPixbuf* px);
    void set_channels(GdkPixbuf* r, GdkPixbuf* g, GdkPixbuf* b);
    void set_brightness_label(double v);
    void set_contrast_label(double v);
    void show();

private:
    void build_ui();
    void build_menu(GtkWidget* vbox);

    GtkWidget *window_{}, *grid_{};
    GtkWidget *images_[6]{}, *labels_[6]{};
    GtkWidget *btn_load_{}, *btn_export_{}, *combo_mode_{};
    GtkWidget *scale_brightness_{}, *scale_contrast_{};
    GtkWidget *lbl_brightness_{}, *lbl_contrast_{}, *lbl_filename_{};
};

} // namespace ui
