#include "AppController.hpp"
#include "operations.hpp"

AppController::AppController(Gtk::Window& win, CanvasArea& canvas, LeftPanel& panel)
: win_(win), canvas_(canvas), panel_(panel) {}

void AppController::on_open_image() {
    Gtk::FileChooserDialog dialog(win_, "Abrir imagen", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Abrir", Gtk::RESPONSE_OK);

    auto filter_img = Gtk::FileFilter::create();
    filter_img->set_name("Imágenes");
    filter_img->add_mime_type("image/png");
    filter_img->add_mime_type("image/jpeg");
    filter_img->add_mime_type("image/jpg");
    dialog.add_filter(filter_img);

    if (dialog.run() == Gtk::RESPONSE_OK) {
        auto filename = dialog.get_filename();
        try {
            auto pix = Gdk::Pixbuf::create_from_file(filename);
            canvas_.add_image(pix);
            panel_.set_current_tool("Imagen cargada");
        } catch (const Glib::Error& ex) {
            Glib::ustring msg = "No se pudo cargar la imagen:\n";
            msg += ex.what();
            Gtk::MessageDialog dlg(win_, msg, false,
                                   Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
            dlg.run();
        }
    }
}

void AppController::on_quit() {
    win_.hide();
}

void AppController::on_color_transforms() {
    Operations::do_color_transforms(win_, canvas_, panel_);
}

void AppController::on_histograma() {
    Operations::do_histogram(win_, canvas_, panel_);
}

void AppController::on_logical_ops() {
    Operations::do_logical_ops(win_, canvas_, panel_);
}

void AppController::on_geom_ops() {
    Operations::do_geom_ops(win_, canvas_, panel_);
}

void AppController::on_morph_ops() {
    Operations::do_morph_ops(win_, canvas_, panel_);
}

void AppController::on_fourier_ops() {
    Operations::do_fourier_ops(win_, canvas_, panel_);
}

void AppController::on_lowpass() {
    Operations::apply_lowpass(win_, canvas_, panel_);
}

void AppController::on_highpass() {
    Operations::apply_highpass(win_, canvas_, panel_);
}

void AppController::on_edge() {
    Operations::apply_edge(win_, canvas_, panel_);
}

void AppController::on_canny() {
    Operations::apply_canny(win_, canvas_, panel_);
}

void AppController::on_about() {
    Glib::ustring msg =
        "Herramientas de Análisis de Imágenes\n\n"
        "• Ventana principal con barra de menús profesional.\n"
        "• Lienzo para colocar múltiples imágenes y moverlas con el ratón.\n"
        "• Columna izquierda dinámica para mostrar la herramienta activa.\n\n"
        "Prácticas integradas:\n"
        "  1) Color básico / grises\n"
        "  2-3) Histograma y ecualización\n"
        "  4) Transformaciones lógicas (binarización)\n"
        "  5) Transformaciones geométricas\n"
        "  6) Operaciones morfológicas\n"
        "  7) Transformada de Fourier\n"
        "  8) Filtrado y detección de bordes (Canny, Sobel, etc.).";
    show_info(msg);
}

void AppController::show_info(const Glib::ustring& msg) {
    Gtk::MessageDialog dlg(win_, msg, false,
                           Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
    dlg.run();
}
