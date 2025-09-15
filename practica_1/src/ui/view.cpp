#include "ui/view.hpp"

namespace ui {

View::View() {
    window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_), "Extractor de Canales RGB - 24 bits");
    gtk_window_set_default_size(GTK_WINDOW(window_), 1000, 700);
    gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
    g_signal_connect(window_, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    build_ui();
}

void View::build_ui() {
    grid_ = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid_), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid_), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(grid_), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid_), 5);
    gtk_container_set_border_width(GTK_CONTAINER(grid_), 10);

    const char* label_texts[] = {
        "Menú", "Imagen Original", "Canal Rojo (8 bits R)",
        "Canal Azul (8 bits B)", "Canal Verde (8 bits G)", "Escala de Grises con Controles"
    };

    for (int i = 0; i < 6; ++i) {
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        labels_[i] = gtk_label_new(label_texts[i]);
        gtk_widget_set_halign(labels_[i], GTK_ALIGN_CENTER);

        if (i == 0) {
            build_menu(vbox);
        } else {
            images_[i] = gtk_image_new();
            gtk_image_set_from_icon_name(GTK_IMAGE(images_[i]), "image-missing", GTK_ICON_SIZE_DIALOG);
            gtk_box_pack_start(GTK_BOX(vbox), labels_[i], FALSE, FALSE, 0);
            gtk_box_pack_start(GTK_BOX(vbox), images_[i], TRUE, TRUE, 0);
        }

        GtkWidget *frame = gtk_frame_new(NULL);
        gtk_container_add(GTK_CONTAINER(frame), vbox);
        int row = i / 3, col = i % 3;
        gtk_grid_attach(GTK_GRID(grid_), frame, col, row, 1, 1);
    }
    gtk_container_add(GTK_CONTAINER(window_), grid_);
}

void View::build_menu(GtkWidget* vbox) {
    gtk_box_set_spacing(GTK_BOX(vbox), 3);
    gtk_box_pack_start(GTK_BOX(vbox), labels_[0], FALSE, FALSE, 0);
    gtk_widget_set_halign(labels_[0], GTK_ALIGN_CENTER);

    // Cargar
    btn_load_ = gtk_button_new_with_label("Seleccionar Imagen");
    gtk_widget_set_halign(btn_load_, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), btn_load_, FALSE, FALSE, 0);

    // Ícono
    images_[0] = gtk_image_new();
    gtk_image_set_from_icon_name(GTK_IMAGE(images_[0]), "image-x-generic", GTK_ICON_SIZE_DIALOG);
    gtk_image_set_pixel_size(GTK_IMAGE(images_[0]), 96);
    gtk_widget_set_halign(images_[0], GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), images_[0], FALSE, FALSE, 0);

    // Nombre de archivo
    lbl_filename_ = gtk_label_new("Sin archivo");
    gtk_widget_set_halign(lbl_filename_, GTK_ALIGN_CENTER);
    gtk_label_set_xalign(GTK_LABEL(lbl_filename_), 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), lbl_filename_, FALSE, FALSE, 0);

    GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator1, FALSE, FALSE, 3);

    // Brillo
    lbl_brightness_ = gtk_label_new("Brillo: 0");
    gtk_widget_set_halign(lbl_brightness_, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), lbl_brightness_, FALSE, FALSE, 0);

    scale_brightness_ = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -100.0, 100.0, 1.0);
    gtk_range_set_value(GTK_RANGE(scale_brightness_), 0.0);
    gtk_scale_set_draw_value(GTK_SCALE(scale_brightness_), FALSE);
    gtk_widget_set_halign(scale_brightness_, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(scale_brightness_, 220, -1);
    gtk_box_pack_start(GTK_BOX(vbox), scale_brightness_, FALSE, FALSE, 0);

    // Contraste
    lbl_contrast_ = gtk_label_new("Contraste: 1.00");
    gtk_widget_set_halign(lbl_contrast_, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), lbl_contrast_, FALSE, FALSE, 0);

    scale_contrast_ = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.1, 2.0, 0.01);
    gtk_range_set_value(GTK_RANGE(scale_contrast_), 1.0);
    gtk_scale_set_draw_value(GTK_SCALE(scale_contrast_), FALSE);
    gtk_widget_set_halign(scale_contrast_, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(scale_contrast_, 220, -1);
    gtk_box_pack_start(GTK_BOX(vbox), scale_contrast_, FALSE, FALSE, 0);

    // Título modo CSV
    GtkWidget *mode_title = gtk_label_new("Modo para exportar a CSV");
    gtk_widget_set_halign(mode_title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), mode_title, FALSE, FALSE, 0);

    // Combo
    combo_mode_ = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_mode_), "Color (0x00RRGGBB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_mode_), "Rojo (0..255)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_mode_), "Verde (0..255)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_mode_), "Azul (0..255)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_mode_), "Gris (0..255)");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_mode_), 0);
    gtk_widget_set_halign(combo_mode_, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(combo_mode_, 220, -1);
    gtk_box_pack_start(GTK_BOX(vbox), combo_mode_, FALSE, FALSE, 2);

    // Exportar CSV
    btn_export_ = gtk_button_new_with_label("Exportar CSV");
    gtk_widget_set_halign(btn_export_, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), btn_export_, FALSE, FALSE, 3);

    // Reset (solo UI; el controlador hará el refresco al cambiar sliders)
    GtkWidget *reset_button = gtk_button_new_with_label("Reset Controles");
    gtk_widget_set_halign(reset_button, GTK_ALIGN_CENTER);
    g_signal_connect_swapped(reset_button, "clicked",
        G_CALLBACK(+[] (View* self){
            gtk_range_set_value(GTK_RANGE(self->scale_brightness()), 0.0);
            gtk_range_set_value(GTK_RANGE(self->scale_contrast()),   1.0);
        }), this);
    gtk_box_pack_start(GTK_BOX(vbox), reset_button, FALSE, FALSE, 3);
}

void View::set_filename(const char* name) {
    gtk_label_set_text(GTK_LABEL(lbl_filename_), name ? name : "Sin archivo");
}

void View::set_original(GdkPixbuf* px) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(images_[1]), px);
}
void View::set_gray(GdkPixbuf* px) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(images_[5]), px);
}
void View::set_channels(GdkPixbuf* r, GdkPixbuf* g, GdkPixbuf* b) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(images_[2]), r);
    gtk_image_set_from_pixbuf(GTK_IMAGE(images_[4]), g);
    gtk_image_set_from_pixbuf(GTK_IMAGE(images_[3]), b);
}

void View::set_brightness_label(double v) {
    char text[64]; snprintf(text, sizeof(text), "Brillo: %.0f", v);
    gtk_label_set_text(GTK_LABEL(lbl_brightness_), text);
}
void View::set_contrast_label(double v) {
    char text[64]; snprintf(text, sizeof(text), "Contraste: %.2f", v);
    gtk_label_set_text(GTK_LABEL(lbl_contrast_), text);
}

void View::show() { gtk_widget_show_all(window_); }

} // namespace ui
