#include "AppWindow.hpp"

AppWindow::AppWindow()
: m_vbox(Gtk::ORIENTATION_VERTICAL),
  m_paned(Gtk::ORIENTATION_HORIZONTAL),
  m_controller(*this, m_canvas, m_leftPanel) {

    // Título de la ventana
    set_title("Herramientas Analisis de Imagenes - Practicas 1 a 8");
    set_default_size(1200, 800);

    add(m_vbox);

    build_menus();
    m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

    // Panel izquierdo (opciones / parámetros) y canvas a la derecha
    m_paned.pack1(m_leftPanel, Gtk::SHRINK);
    m_paned.pack2(m_canvas, Gtk::EXPAND);

    m_vbox.pack_start(m_paned, Gtk::PACK_EXPAND_WIDGET);

    show_all_children();
}

void AppWindow::build_menus() {
    // ===== Archivo =====
    auto item_file = Gtk::make_managed<Gtk::MenuItem>("_Archivo", true);
    auto menu_file = Gtk::make_managed<Gtk::Menu>();

    auto item_open = Gtk::make_managed<Gtk::MenuItem>("_Abrir imagen...", true);
    item_open->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_open_image));
    menu_file->append(*item_open);

    auto item_quit = Gtk::make_managed<Gtk::MenuItem>("_Salir", true);
    item_quit->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_quit));
    menu_file->append(*item_quit);

    item_file->set_submenu(*menu_file);
    m_menubar.append(*item_file);

    // ===== Practica 1: Color =====
    auto item_p1 = Gtk::make_managed<Gtk::MenuItem>("P1 _Color", true);
    auto menu_p1 = Gtk::make_managed<Gtk::Menu>();
    auto item_p1_color = Gtk::make_managed<Gtk::MenuItem>("Transformaciones de color...", true);
    item_p1_color->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_color_transforms));
    menu_p1->append(*item_p1_color);
    item_p1->set_submenu(*menu_p1);
    m_menubar.append(*item_p1);

    // ===== Practicas 2-3: Histograma =====
    auto item_p23 = Gtk::make_managed<Gtk::MenuItem>("P2-3 _Histograma", true);
    auto menu_p23 = Gtk::make_managed<Gtk::Menu>();
    auto item_p23_hist = Gtk::make_managed<Gtk::MenuItem>("Ecualización de histograma (grises)", true);
    item_p23_hist->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_histograma));
    menu_p23->append(*item_p23_hist);
    item_p23->set_submenu(*menu_p23);
    m_menubar.append(*item_p23);

    // ===== Practica 4: Lógicas =====
    auto item_p4 = Gtk::make_managed<Gtk::MenuItem>("P4 _Lógicas", true);
    auto menu_p4 = Gtk::make_managed<Gtk::Menu>();
    auto item_p4_thr = Gtk::make_managed<Gtk::MenuItem>("Umbral binario", true);
    item_p4_thr->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_logical_ops));
    menu_p4->append(*item_p4_thr);
    item_p4->set_submenu(*menu_p4);
    m_menubar.append(*item_p4);

    // ===== Practica 5: Geometricas =====
    auto item_p5 = Gtk::make_managed<Gtk::MenuItem>("P5 _Geométricas", true);
    auto menu_p5 = Gtk::make_managed<Gtk::Menu>();
    auto item_p5_geom = Gtk::make_managed<Gtk::MenuItem>("Transformaciones geométricas...", true);
    item_p5_geom->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_geom_ops));
    menu_p5->append(*item_p5_geom);
    item_p5->set_submenu(*menu_p5);
    m_menubar.append(*item_p5);

    // ===== Practica 6: Morfologia =====
    auto item_p6 = Gtk::make_managed<Gtk::MenuItem>("P6 _Morfología", true);
    auto menu_p6 = Gtk::make_managed<Gtk::Menu>();
    auto item_p6_morph = Gtk::make_managed<Gtk::MenuItem>("Operaciones morfológicas...", true);
    item_p6_morph->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_morph_ops));
    menu_p6->append(*item_p6_morph);
    item_p6->set_submenu(*menu_p6);
    m_menubar.append(*item_p6);

    // ===== Practica 7: Fourier =====
    auto item_p7 = Gtk::make_managed<Gtk::MenuItem>("P7 _Fourier", true);
    auto menu_p7 = Gtk::make_managed<Gtk::Menu>();
    auto item_p7_fft = Gtk::make_managed<Gtk::MenuItem>("Transformada de Fourier 2D...", true);
    item_p7_fft->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_fourier_ops));
    menu_p7->append(*item_p7_fft);
    item_p7->set_submenu(*menu_p7);
    m_menubar.append(*item_p7);

    // ===== Practica 8: Filtros y bordes =====
    auto item_p8 = Gtk::make_managed<Gtk::MenuItem>("P8 _Filtros/Bordes", true);
    auto menu_p8 = Gtk::make_managed<Gtk::Menu>();

    auto item_low = Gtk::make_managed<Gtk::MenuItem>("Filtro pasa bajas (Gauss)", true);
    item_low->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_lowpass));
    menu_p8->append(*item_low);

    auto item_high = Gtk::make_managed<Gtk::MenuItem>("Filtro de realce (High-pass)", true);
    item_high->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_highpass));
    menu_p8->append(*item_high);

    auto item_edge = Gtk::make_managed<Gtk::MenuItem>("Detección de bordes (gradiente)", true);
    item_edge->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_edge));
    menu_p8->append(*item_edge);

    auto item_canny = Gtk::make_managed<Gtk::MenuItem>("Detector de Canny", true);
    item_canny->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_canny));
    menu_p8->append(*item_canny);

    item_p8->set_submenu(*menu_p8);
    m_menubar.append(*item_p8);

    // ===== Ayuda =====
    auto item_help = Gtk::make_managed<Gtk::MenuItem>("_Ayuda", true);
    auto menu_help = Gtk::make_managed<Gtk::Menu>();
    auto item_about = Gtk::make_managed<Gtk::MenuItem>("Acerca de...", true);
    item_about->signal_activate().connect(
        sigc::mem_fun(m_controller, &AppController::on_about));
    menu_help->append(*item_about);
    item_help->set_submenu(*menu_help);
    m_menubar.append(*item_help);
}
