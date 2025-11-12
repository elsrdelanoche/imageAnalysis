#include "AppController.hpp"
AppController::AppController(std::shared_ptr<ImageModel> model, MainWindow& view)
: model_(std::move(model)), view_(view){
    view_.btn_load().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_load));
    view_.btn_save().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_save));
    view_.btn_apply().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_apply));
    view_.btn_canny().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_canny));
}
void AppController::on_load(){
    Gtk::FileChooserDialog dlg("Selecciona imagen", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dlg.set_transient_for(view_); dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL); dlg.add_button("_Abrir", Gtk::RESPONSE_OK);
    auto filter = Gtk::FileFilter::create(); filter->add_pixbuf_formats(); filter->set_name("Imágenes"); dlg.add_filter(filter);
    if(dlg.run()==Gtk::RESPONSE_OK){
        std::string err; if(!model_->load_image(dlg.get_filename(), err)){
            Gtk::MessageDialog m(view_, "No se pudo cargar:\n"+err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
        } view_.refresh();
    }
}
void AppController::on_save(){
    Gtk::FileChooserDialog dlg("Guardar resultado", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dlg.set_transient_for(view_); dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL); dlg.add_button("_Guardar", Gtk::RESPONSE_OK);
    dlg.set_current_name("resultado.png");
    if(dlg.run()==Gtk::RESPONSE_OK){
        std::string err; if(!model_->save_result(dlg.get_filename(), err)){
            Gtk::MessageDialog m(view_, "No se pudo guardar:\n"+err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
        }
    }
}
void AppController::on_apply(){
    auto cat = view_.cb_cat().get_active_text(); auto name = view_.cb_kernel().get_active_text();
    Params p; p.k_gauss=view_.sp_gauss_k().get_value_as_int(); if(p.k_gauss%2==0) p.k_gauss+=1;
    p.sigma=view_.sp_sigma().get_value(); p.alpha=view_.sp_alpha().get_value();
    std::string err; bool ok=false;
    if(cat=="Pasa-bajas"){ ok=model_->apply_lowpass(name,p,err); }
    else if(cat=="Pasa-altas"){ ok=model_->apply_highpass(name,p,err); }
    else if(cat=="Bordes"){ ok=model_->apply_edge(name,err); }
    if(!ok && !err.empty()){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_canny(){
    Params p; p.k_canny=view_.sp_canny_k().get_value_as_int(); if(p.k_canny%2==0) p.k_canny+=1;
    p.sigma_c=view_.sp_sigma_c().get_value(); p.t_low=view_.sp_tlow().get_value(); p.t_high=view_.sp_thigh().get_value();
    std::string err; if(!model_->apply_canny(p,err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
