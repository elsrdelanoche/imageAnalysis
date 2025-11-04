#include "AppController.hpp"
#include <iostream>

AppController::AppController(std::shared_ptr<ImageModel> model, MainWindow& view)
: model_(std::move(model)), view_(view) {
    view_.btn_load_a().signal_clicked().connect(sigc::mem_fun(*this, &AppController::on_load_a));
    view_.btn_load_b().signal_clicked().connect(sigc::mem_fun(*this, &AppController::on_load_b));
    view_.btn_apply().signal_clicked().connect(sigc::mem_fun(*this, &AppController::on_apply));
    view_.btn_save().signal_clicked().connect(sigc::mem_fun(*this, &AppController::on_save));
}

void AppController::show_error(const std::string& msg){
    Gtk::MessageDialog dlg(view_, msg, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    dlg.run();
}

void AppController::on_load_a(){
    Gtk::FileChooserDialog dlg(view_, "Selecciona imagen A", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Abrir", Gtk::RESPONSE_OK);
    auto filter = Gtk::FileFilter::create();
    filter->add_pixbuf_formats();
    filter->set_name("Imágenes");
    dlg.add_filter(filter);
    if(dlg.run() == Gtk::RESPONSE_OK){
        std::string err;
        if(!model_->load_image_a(dlg.get_filename(), err)) show_error("No se pudo cargar A:\n"+err);
        model_->set_force_binarize(view_.chk_binarize().get_active());
        view_.refresh_images();
    }
}

void AppController::on_load_b(){
    Gtk::FileChooserDialog dlg(view_, "Selecciona imagen B", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Abrir", Gtk::RESPONSE_OK);
    auto filter = Gtk::FileFilter::create();
    filter->add_pixbuf_formats();
    filter->set_name("Imágenes");
    dlg.add_filter(filter);
    if(dlg.run() == Gtk::RESPONSE_OK){
        std::string err;
        if(!model_->load_image_b(dlg.get_filename(), err)) show_error("No se pudo cargar B:\n"+err);
        model_->set_force_binarize(view_.chk_binarize().get_active());
        view_.refresh_images();
    }
}

void AppController::on_apply(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err;
    // Determine op
    if(view_.rb_and().get_active() || view_.rb_or().get_active() || view_.rb_xor().get_active()){
        LogicalOp op = LogicalOp::AND;
        if(view_.rb_or().get_active()) op = LogicalOp::OR;
        if(view_.rb_xor().get_active()) op = LogicalOp::XOR;
        bool not_a = view_.chk_not_a().get_active();
        bool not_b = view_.chk_not_b().get_active();
        if(!model_->apply_logical(op, not_a, not_b, err)) { show_error(err); return; }
    } else {
        RelOp op = RelOp::EQ;
        if(view_.rb_ne().get_active()) op = RelOp::NE;
        else if(view_.rb_gt().get_active()) op = RelOp::GT;
        else if(view_.rb_ge().get_active()) op = RelOp::GE;
        else if(view_.rb_lt().get_active()) op = RelOp::LT;
        else if(view_.rb_le().get_active()) op = RelOp::LE;
        if(!model_->apply_relational(op, err)) { show_error(err); return; }
    }
    view_.refresh_images();
}

void AppController::on_save(){
    Gtk::FileChooserDialog dlg(view_, "Guardar resultado", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Guardar", Gtk::RESPONSE_OK);
    dlg.set_current_name("resultado.png");
    if(dlg.run() == Gtk::RESPONSE_OK){
        std::string err;
        if(!model_->save_result(dlg.get_filename(), err)) show_error("No se pudo guardar:\n"+err);
    }
}
