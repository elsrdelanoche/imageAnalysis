#include "view/MainWindow.h"
#include "controller/TransformController.h"
#include <iostream>

MainWindow::MainWindow() 
    : main_box_(Gtk::ORIENTATION_VERTICAL),
      content_box_(Gtk::ORIENTATION_HORIZONTAL),
      transform_panel_(Gtk::ORIENTATION_VERTICAL, 10) {
    
    set_title("Transformaciones Geométricas de Imágenes");
    set_default_size(1200, 800);
    
    setupUI();
}

void MainWindow::setController(std::shared_ptr<TransformController> controller) {
    controller_ = controller;
}

void MainWindow::setupUI() {
    add(main_box_);
    
    createMenuBar();
    createToolbar();
    createMainArea();
    
    main_box_.pack_start(statusbar_, Gtk::PACK_SHRINK);
    statusbar_.push("Listo. Abra una imagen para comenzar.");
    
    show_all_children();
}

void MainWindow::createMenuBar() {
    auto menubar = Gtk::manage(new Gtk::MenuBar());
    auto menu_file = Gtk::manage(new Gtk::Menu());
    auto item_file = Gtk::manage(new Gtk::MenuItem("_Archivo", true));
    item_file->set_submenu(*menu_file);
    
    auto item_open = Gtk::manage(new Gtk::MenuItem("_Abrir imagen", true));
    item_open->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onOpenImage));
    menu_file->append(*item_open);
    
    auto item_save = Gtk::manage(new Gtk::MenuItem("_Guardar imagen", true));
    item_save->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onSaveImage));
    menu_file->append(*item_save);
    
    menu_file->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    
    auto item_quit = Gtk::manage(new Gtk::MenuItem("_Salir", true));
    item_quit->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::onQuit));
    menu_file->append(*item_quit);
    
    menubar->append(*item_file);
    main_box_.pack_start(*menubar, Gtk::PACK_SHRINK);
}

void MainWindow::createToolbar() {
    auto toolbar = Gtk::manage(new Gtk::Toolbar());
    toolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
    
    auto btn_open = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::OPEN));
    btn_open->set_tooltip_text("Abrir imagen");
    btn_open->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onOpenImage));
    toolbar->append(*btn_open);
    
    auto btn_save = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::SAVE));
    btn_save->set_tooltip_text("Guardar imagen");
    btn_save->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onSaveImage));
    toolbar->append(*btn_save);
    
    toolbar->append(*Gtk::manage(new Gtk::SeparatorToolItem()));
    
    auto btn_reset = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::REFRESH));
    btn_reset->set_tooltip_text("Restaurar imagen original");
    btn_reset->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onResetImage));
    toolbar->append(*btn_reset);
    
    main_box_.pack_start(*toolbar, Gtk::PACK_SHRINK);
}

void MainWindow::createMainArea() {
    main_box_.pack_start(content_box_, Gtk::PACK_EXPAND_WIDGET);
    
    scrolled_window_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrolled_window_.add(image_display_);
    content_box_.pack_start(scrolled_window_, Gtk::PACK_EXPAND_WIDGET);
    
    createTransformPanel();
    content_box_.pack_start(transform_panel_, Gtk::PACK_SHRINK);
}

void MainWindow::createTransformPanel() {
    transform_panel_.set_size_request(300, -1);
    transform_panel_.set_border_width(10);
    
    auto title = Gtk::manage(new Gtk::Label());
    title->set_markup("<b>Transformaciones</b>");
    transform_panel_.pack_start(*title, Gtk::PACK_SHRINK);
    
    // TRASLACIÓN
    translation_frame_.set_label("Traslación");
    translation_frame_.set_border_width(5);
    
    auto trans_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 5));
    trans_box->set_border_width(10);
    
    auto tx_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    tx_box->pack_start(*Gtk::manage(new Gtk::Label("Tx (píxeles):")), Gtk::PACK_SHRINK);
    spin_tx_.set_range(-1000, 1000);
    spin_tx_.set_increments(1, 10);
    spin_tx_.set_value(0);
    tx_box->pack_start(spin_tx_, Gtk::PACK_EXPAND_WIDGET);
    trans_box->pack_start(*tx_box, Gtk::PACK_SHRINK);
    
    auto ty_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    ty_box->pack_start(*Gtk::manage(new Gtk::Label("Ty (píxeles):")), Gtk::PACK_SHRINK);
    spin_ty_.set_range(-1000, 1000);
    spin_ty_.set_increments(1, 10);
    spin_ty_.set_value(0);
    ty_box->pack_start(spin_ty_, Gtk::PACK_EXPAND_WIDGET);
    trans_box->pack_start(*ty_box, Gtk::PACK_SHRINK);
    
    btn_apply_translation_.set_label("Aplicar Traslación");
    btn_apply_translation_.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onApplyTranslation));
    trans_box->pack_start(btn_apply_translation_, Gtk::PACK_SHRINK);
    
    translation_frame_.add(*trans_box);
    transform_panel_.pack_start(translation_frame_, Gtk::PACK_SHRINK);
    
    // ROTACIÓN
    rotation_frame_.set_label("Rotación");
    rotation_frame_.set_border_width(5);
    
    auto rot_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 5));
    rot_box->set_border_width(10);
    
    auto angle_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    angle_box->pack_start(*Gtk::manage(new Gtk::Label("Ángulo (°):")), Gtk::PACK_SHRINK);
    spin_angle_.set_range(-360, 360);
    spin_angle_.set_increments(1, 15);
    spin_angle_.set_value(0);
    angle_box->pack_start(spin_angle_, Gtk::PACK_EXPAND_WIDGET);
    rot_box->pack_start(*angle_box, Gtk::PACK_SHRINK);
    
    auto note = Gtk::manage(new Gtk::Label("+ horario, - antihorario"));
    note->set_line_wrap(true);
    rot_box->pack_start(*note, Gtk::PACK_SHRINK);
    
    btn_apply_rotation_.set_label("Aplicar Rotación");
    btn_apply_rotation_.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onApplyRotation));
    rot_box->pack_start(btn_apply_rotation_, Gtk::PACK_SHRINK);
    
    rotation_frame_.add(*rot_box);
    transform_panel_.pack_start(rotation_frame_, Gtk::PACK_SHRINK);
    
    // ESCALAMIENTO
    scale_frame_.set_label("Escalamiento");
    scale_frame_.set_border_width(5);
    
    auto scale_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 5));
    scale_box->set_border_width(10);
    
    auto sx_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    sx_box->pack_start(*Gtk::manage(new Gtk::Label("Factor X:")), Gtk::PACK_SHRINK);
    spin_sx_.set_range(0.1, 10.0);
    spin_sx_.set_increments(0.1, 0.5);
    spin_sx_.set_value(1.0);
    spin_sx_.set_digits(2);
    sx_box->pack_start(spin_sx_, Gtk::PACK_EXPAND_WIDGET);
    scale_box->pack_start(*sx_box, Gtk::PACK_SHRINK);
    
    auto sy_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    sy_box->pack_start(*Gtk::manage(new Gtk::Label("Factor Y:")), Gtk::PACK_SHRINK);
    spin_sy_.set_range(0.1, 10.0);
    spin_sy_.set_increments(0.1, 0.5);
    spin_sy_.set_value(1.0);
    spin_sy_.set_digits(2);
    sy_box->pack_start(spin_sy_, Gtk::PACK_EXPAND_WIDGET);
    scale_box->pack_start(*sy_box, Gtk::PACK_SHRINK);
    
    auto interp_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    interp_box->pack_start(*Gtk::manage(new Gtk::Label("Interpolación:")), Gtk::PACK_SHRINK);
    combo_interpolation_.append("Vecino más cercano");
    combo_interpolation_.append("Bilineal");
    combo_interpolation_.append("Bicúbica");
    combo_interpolation_.set_active(2);
    interp_box->pack_start(combo_interpolation_, Gtk::PACK_EXPAND_WIDGET);
    scale_box->pack_start(*interp_box, Gtk::PACK_SHRINK);
    
    btn_apply_scale_.set_label("Aplicar Escalamiento");
    btn_apply_scale_.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::onApplyScale));
    scale_box->pack_start(btn_apply_scale_, Gtk::PACK_SHRINK);
    
    scale_frame_.add(*scale_box);
    transform_panel_.pack_start(scale_frame_, Gtk::PACK_SHRINK);
    
    transform_panel_.pack_start(*Gtk::manage(new Gtk::Separator()), Gtk::PACK_SHRINK);
    btn_reset_.set_label("Restaurar Original");
    btn_reset_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onResetImage));
    transform_panel_.pack_start(btn_reset_, Gtk::PACK_SHRINK);
}

void MainWindow::displayImage(Glib::RefPtr<Gdk::Pixbuf> pixbuf) {
    if (pixbuf) {
        image_display_.set(pixbuf);
    }
}

void MainWindow::updateStatus(const std::string& message) {
    statusbar_.pop();
    statusbar_.push(message);
}

void MainWindow::onOpenImage() {
    Gtk::FileChooserDialog dialog("Abrir imagen", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);
    
    dialog.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Abrir", Gtk::RESPONSE_OK);
    
    auto filter_image = Gtk::FileFilter::create();
    filter_image->set_name("Imágenes");
    filter_image->add_mime_type("image/png");
    filter_image->add_mime_type("image/jpeg");
    filter_image->add_mime_type("image/bmp");
    dialog.add_filter(filter_image);
    
    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK && controller_) {
        controller_->loadImage(dialog.get_filename());
    }
}

void MainWindow::onSaveImage() {
    if (controller_) {
        Gtk::FileChooserDialog dialog("Guardar imagen", Gtk::FILE_CHOOSER_ACTION_SAVE);
        dialog.set_transient_for(*this);
        
        dialog.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Guardar", Gtk::RESPONSE_OK);
        
        dialog.set_do_overwrite_confirmation(true);
        
        auto filter_png = Gtk::FileFilter::create();
        filter_png->set_name("PNG");
        filter_png->add_pattern("*.png");
        dialog.add_filter(filter_png);
        
        auto filter_jpg = Gtk::FileFilter::create();
        filter_jpg->set_name("JPEG");
        filter_jpg->add_pattern("*.jpg");
        filter_jpg->add_pattern("*.jpeg");
        dialog.add_filter(filter_jpg);
        
        int result = dialog.run();
        if (result == Gtk::RESPONSE_OK) {
            controller_->saveImage(dialog.get_filename());
        }
    }
}

void MainWindow::onApplyTranslation() {
    if (controller_) controller_->applyTranslation();
}

void MainWindow::onApplyRotation() {
    if (controller_) controller_->applyRotation();
}

void MainWindow::onApplyScale() {
    if (controller_) controller_->applyScale();
}

void MainWindow::onResetImage() {
    if (controller_) controller_->resetImage();
}

void MainWindow::onQuit() {
    hide();
}

double MainWindow::getTranslationX() const { return spin_tx_.get_value(); }
double MainWindow::getTranslationY() const { return spin_ty_.get_value(); }
double MainWindow::getRotationAngle() const { return spin_angle_.get_value(); }
double MainWindow::getScaleX() const { return spin_sx_.get_value(); }
double MainWindow::getScaleY() const { return spin_sy_.get_value(); }
int MainWindow::getInterpolationType() const { return combo_interpolation_.get_active_row_number(); }
