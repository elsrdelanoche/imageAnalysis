
#include "ui/view.hpp"

namespace ui {

static GtkWidget* titled_frame(const char* title, GtkWidget* child){
    GtkWidget* frame = gtk_frame_new(title);
    gtk_container_add(GTK_CONTAINER(frame), child);
    return frame;
}

View::View() {
    window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_), "Práctica: Histograma y Ecualización");
    gtk_window_set_default_size(GTK_WINDOW(window_), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
    g_signal_connect(window_, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    build_ui();
}

void View::build_ui() {
    GtkWidget* root_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_container_add(GTK_CONTAINER(window_), root_box);

    // Left panel
    left_panel_ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_size_request(left_panel_, 340, -1);
    gtk_box_pack_start(GTK_BOX(root_box), left_panel_, FALSE, FALSE, 0);

    btn_open_ = gtk_button_new_with_label("📂 Cargar imagen...");
    gtk_box_pack_start(GTK_BOX(left_panel_), btn_open_, FALSE, FALSE, 0);

    // Fixed-size input image
    img_input_ = gtk_image_new();
    GtkWidget* left_img_frame = gtk_aspect_frame_new("Imagen de entrada", 0.5, 0.5, (double)LEFT_IMG_W/LEFT_IMG_H, FALSE);
    gtk_widget_set_size_request(left_img_frame, LEFT_IMG_W+20, LEFT_IMG_H+20);
    gtk_container_add(GTK_CONTAINER(left_img_frame), img_input_);
    gtk_box_pack_start(GTK_BOX(left_panel_), left_img_frame, FALSE, FALSE, 0);

    GtkWidget* frm = gtk_frame_new("Transformación (Tabla 2.1)");
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(frm), box);
    cmb_transform_ = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmb_transform_), "Negativo s=255-r");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmb_transform_), "Logarítmica s=c*log(1+r)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmb_transform_), "Potencia (Gamma) s=c*r^γ");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmb_transform_), "Estiramiento Lineal");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmb_transform_), 2);
    gtk_box_pack_start(GTK_BOX(box), cmb_transform_, FALSE, FALSE, 0);

    scale_gamma_ = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.1, 3.0, 0.05);
    gtk_scale_set_value_pos(GTK_SCALE(scale_gamma_), GTK_POS_RIGHT);
    gtk_range_set_value(GTK_RANGE(scale_gamma_), 1.0);
    gtk_box_pack_start(GTK_BOX(box), scale_gamma_, FALSE, FALSE, 0);

    btn_apply_ = gtk_button_new_with_label("➕ Aplicar transformación");
    gtk_box_pack_start(GTK_BOX(box), btn_apply_, FALSE, FALSE, 0);

    btn_equalize_ = gtk_button_new_with_label("⇆ Ecualizar por CDF");
    gtk_box_pack_start(GTK_BOX(left_panel_), btn_equalize_, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(left_panel_), frm, FALSE, FALSE, 0);

    // Right with scroller
    right_panel_ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(root_box), right_panel_, TRUE, TRUE, 0);

    GtkWidget* scroller = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(right_panel_), scroller, TRUE, TRUE, 0);

    grid_right_ = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_right_), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid_right_), 6);
    gtk_container_add(GTK_CONTAINER(scroller), grid_right_);

    img_hist_ = gtk_image_new();
    img_pdf_  = gtk_image_new();
    img_cdf_  = gtk_image_new();
    img_equalized_ = gtk_image_new();
    img_pdf_eq_ = gtk_image_new();
    img_custom_ = gtk_image_new();
    img_hist_custom_ = gtk_image_new();
    stats_view_ = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(stats_view_), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(stats_view_), GTK_WRAP_WORD_CHAR);

    // 2 columns per row, order per request
    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("1) Histograma h[i]", img_hist_), 0,0,1,1);
    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("2) \"Histograma phi de la imagen de entrada\"", img_pdf_), 1,0,1,1);

    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("3) \"Densidad Dpi de la imagen entrapa\"", img_cdf_), 0,1,1,1);
    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("4) Propiedades estadísticas", stats_view_), 1,1,1,1);

    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("5) Imagen ecualizada (CDF)", img_equalized_), 0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("5b) φ[i] de imagen ecualizada", img_pdf_eq_), 1,2,1,1);

    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("6) Imagen transformada (Tabla 2.1)", img_custom_), 0,3,1,1);
    gtk_grid_attach(GTK_GRID(grid_right_), titled_frame("6b) Histograma transformado", img_hist_custom_), 1,3,1,1);
}

void View::set_hist_pixbuf(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_hist_), px); }
void View::set_pdf_pixbuf(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_pdf_), px); }
void View::set_cdf_pixbuf(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_cdf_), px); }
void View::set_equalized_image(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_equalized_), px); }
void View::set_pdf_eq_pixbuf(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_pdf_eq_), px); }
void View::set_custom_image(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_custom_), px); }
void View::set_custom_hist(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_hist_custom_), px); }
void View::set_input_image(GdkPixbuf* px){ gtk_image_set_from_pixbuf(GTK_IMAGE(img_input_), px); }
void View::set_stats_text(const std::string& t){
    GtkTextBuffer* buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(stats_view_));
    gtk_text_buffer_set_text(buf, t.c_str(), -1);
}

} // namespace ui
