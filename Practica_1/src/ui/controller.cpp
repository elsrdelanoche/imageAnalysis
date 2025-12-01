#include "ui/controller.hpp"
#include <gtk/gtk.h>

namespace ui {

Controller::~Controller() {
    // core::ImageBuffer maneja su propia memoria (std::vector), no hay GObjects aquí
}

void Controller::connect_signals() {
    g_signal_connect(view_.btn_load(),    "clicked",       G_CALLBACK(on_load),      this);
    g_signal_connect(view_.btn_export(),  "clicked",       G_CALLBACK(on_export),    this);
    g_signal_connect(view_.scale_brightness(), "value-changed", G_CALLBACK(on_brightness), this);
    g_signal_connect(view_.scale_contrast(),   "value-changed", G_CALLBACK(on_contrast),   this);
}

// === Señales estáticas ===
void Controller::on_load(GtkWidget*, gpointer self)    { static_cast<Controller*>(self)->load_dialog(); }
void Controller::on_export(GtkWidget*, gpointer self)  { static_cast<Controller*>(self)->export_dialog(); }
void Controller::on_brightness(GtkRange* r, gpointer self) {
    auto* c = static_cast<Controller*>(self);
    c->brightness_ = gtk_range_get_value(r);
    c->view_.set_brightness_label(c->brightness_);
    c->refresh_color();
    c->refresh_gray();
}
void Controller::on_contrast(GtkRange* r, gpointer self) {
    auto* c = static_cast<Controller*>(self);
    c->contrast_ = gtk_range_get_value(r);
    c->view_.set_contrast_label(c->contrast_);
    c->refresh_color();
    c->refresh_gray();
}

// === Flujo ===
void Controller::load_dialog() {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Seleccionar Imagen",
        GTK_WINDOW(view_.window()),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        "_Abrir", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Imágenes");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.bmp");
    gtk_file_filter_add_pattern(filter, "*.gif");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        process_loaded(filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void Controller::process_loaded(const char* path) {
    // Cargar archivo a buffer (RGB)
    core::ImageBuffer original;
    if (!io::load_image_to_buffer(path, original)) {
        GtkWidget *md = gtk_message_dialog_new(
            GTK_WINDOW(view_.window()), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "No se pudo cargar la imagen.");
        gtk_dialog_run(GTK_DIALOG(md));
        gtk_widget_destroy(md);
        return;
    }

    // Nombre de archivo (basename)
    char *base = g_path_get_basename(path);
    view_.set_filename(base ? base : "Sin archivo");
    if (base) g_free(base);

    // Reescalar a 250x200 para UI (pipeline de resize)
    core::Pipeline p;
    p.add<ops::Resize>(250, 200);
    base_color_ = p.run(original.view()); // salida RGB

    // Crear base gris (Average) para futuros ajustes
    core::Pipeline pg;
    pg.add<ops::Grayscale>(ops::GrayMethod::Average);
    base_gray_ = pg.run(base_color_.view());

    // Mostrar ORIGINAL escalada (sin ajuste aún)
    if (auto* px = model::to_pixbuf(base_color_.view())) {
        view_.set_original(px);
        g_object_unref(px);
    }

    // Mostrar canales R,G,B desde base_color_
    core::Pipeline pr; pr.add<ops::ChannelExtractor>(ops::Channel::R);
    core::Pipeline pgc; pgc.add<ops::ChannelExtractor>(ops::Channel::G);
    core::Pipeline pb; pb.add<ops::ChannelExtractor>(ops::Channel::B);

    auto rr = pr.run(base_color_.view());
    auto gg = pgc.run(base_color_.view());
    auto bb = pb.run(base_color_.view());

    GdkPixbuf *prx = model::to_pixbuf(rr.view());
    GdkPixbuf *pgx = model::to_pixbuf(gg.view());
    GdkPixbuf *pbx = model::to_pixbuf(bb.view());
    view_.set_channels(prx, pgx, pbx);
    if (prx) g_object_unref(prx);
    if (pgx) g_object_unref(pgx);
    if (pbx) g_object_unref(pbx);

    // Ajustes actuales a color y gris
    refresh_color();
    refresh_gray();
}

void Controller::refresh_color() {
    if (!base_color_.valid()) return;
    core::Pipeline p;
    p.add<ops::BrightnessContrast>(brightness_, contrast_);
    auto adjusted = p.run(base_color_.view());

    if (auto* px = model::to_pixbuf(adjusted.view())) {
        view_.set_original(px);
        g_object_unref(px);
    }
}

void Controller::refresh_gray() {
    if (!base_gray_.valid()) return;
    core::Pipeline p;
    p.add<ops::BrightnessContrast>(brightness_, contrast_);
    auto adjusted = p.run(base_gray_.view());

    if (auto* px = model::to_pixbuf(adjusted.view())) {
        view_.set_gray(px);
        g_object_unref(px);
    }
}

void Controller::export_dialog() {
    if (!base_color_.valid()) return;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Guardar CSV",
        GTK_WINDOW(view_.window()),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        "_Guardar", GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "matriz.csv");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        bool ok = io::export_csv(filepath, base_color_.view(), mode_from_combo());
        if (!ok) {
            GtkWidget *md = gtk_message_dialog_new(
                GTK_WINDOW(view_.window()), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                "No se pudo exportar el CSV."
            );
            gtk_dialog_run(GTK_DIALOG(md));
            gtk_widget_destroy(md);
        }
        g_free(filepath);
    }
    gtk_widget_destroy(dialog);
}

io::IntImageMode Controller::mode_from_combo() const {
    int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(view_.combo_mode()));
    switch (idx) {
        case 0: return io::IntImageMode::COLOR_RGB_PACKED;
        case 1: return io::IntImageMode::RED_8U;
        case 2: return io::IntImageMode::GREEN_8U;
        case 3: return io::IntImageMode::BLUE_8U;
        case 4: return io::IntImageMode::GRAY_8U;
        default: return io::IntImageMode::COLOR_RGB_PACKED;
    }
}

} // namespace ui
