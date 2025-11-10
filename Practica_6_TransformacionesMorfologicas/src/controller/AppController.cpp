#include "AppController.hpp"

AppController::AppController(std::shared_ptr<ImageModel> model, MainWindow& view)
: model_(std::move(model)), view_(view)
{
    view_.btn_load().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_load));
    view_.btn_save().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_save));

    view_.cb_shape().signal_changed().connect(sigc::mem_fun(*this,&AppController::on_change_se));
    view_.sp_size().signal_value_changed().connect(sigc::mem_fun(*this,&AppController::on_change_se));

    view_.btn_erode().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_erode));
    view_.btn_dilate().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_dilate));
    view_.btn_open().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_open));
    view_.btn_close().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_close));

    view_.btn_add_salt().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_add_salt));
    view_.btn_add_pepper().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_add_pepper));
    view_.btn_remove_salt().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_remove_salt));
    view_.btn_remove_pepper().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_remove_pepper));

    view_.cb_shape().set_active(0);
    on_change_se();
}

void AppController::on_load(){
    Gtk::FileChooserDialog dlg("Selecciona imagen", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dlg.set_transient_for(view_);
    dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Abrir", Gtk::RESPONSE_OK);
    auto filter = Gtk::FileFilter::create(); filter->add_pixbuf_formats(); filter->set_name("Imágenes"); dlg.add_filter(filter);
    if(dlg.run()==Gtk::RESPONSE_OK){
        std::string err;
        if(!model_->load_image(dlg.get_filename(), err)){
            Gtk::MessageDialog m(view_, "No se pudo cargar:\n"+err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
        }
        model_->set_force_binarize(view_.chk_binarize().get_active());
        view_.refresh();
    }
}

void AppController::on_save(){
    Gtk::FileChooserDialog dlg("Guardar resultado", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dlg.set_transient_for(view_);
    dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Guardar", Gtk::RESPONSE_OK);
    dlg.set_current_name("resultado.png");
    if(dlg.run()==Gtk::RESPONSE_OK){
        std::string err;
        if(!model_->save_out(dlg.get_filename(), err)){
            Gtk::MessageDialog m(view_, "No se pudo guardar:\n"+err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
        }
    }
}

void AppController::on_change_se(){
    int idx = view_.cb_shape().get_active_row_number();
    SEShape shape = SEShape::Square;
    if(idx==1) shape = SEShape::Diamond;
    else if(idx==2) shape = SEShape::Disk;
    int k = view_.sp_size().get_value_as_int();
    if(k%2==0) k+=1;
    model_->set_se(shape, k);
}

void AppController::on_erode(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err; if(!model_->erode(err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_dilate(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err; if(!model_->dilate(err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_open(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err; if(!model_->open(err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_close(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err; if(!model_->close(err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}

double AppController::current_prob() const{
    try { return view_.scale_prob().get_value(); } catch(...) { return 0.05; }
}
void AppController::on_add_salt(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err; if(!model_->add_salt(current_prob(), err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_add_pepper(){
    model_->set_force_binarize(view_.chk_binarize().get_active());
    std::string err; if(!model_->add_pepper(current_prob(), err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_remove_salt(){
    on_open();
}
void AppController::on_remove_pepper(){
    on_close();
}
