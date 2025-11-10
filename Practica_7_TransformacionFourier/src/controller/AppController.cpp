#include "AppController.hpp"

AppController::AppController(std::shared_ptr<ImageModel> model, MainWindow& view)
: model_(std::move(model)), view_(view)
{
    view_.btn_load().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_load));
    view_.btn_save().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_save));
    view_.btn_fft().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_fft));
    view_.btn_ifft().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_ifft));
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
        view_.refresh();
    }
}

void AppController::on_save(){
    Gtk::FileChooserDialog dlg("Guardar resultado", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dlg.set_transient_for(view_);
    dlg.add_button("_Cancelar", Gtk::RESPONSE_CANCEL);
    dlg.add_button("_Guardar", Gtk::RESPONSE_OK);
    dlg.set_current_name("ifft_result.png");
    if(dlg.run()==Gtk::RESPONSE_OK){
        std::string err;
        if(!model_->save_result(dlg.get_filename(), err)){
            Gtk::MessageDialog m(view_, "No se pudo guardar:\n"+err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
        }
    }
}

void AppController::on_fft(){
    std::string err;
    bool shift = view_.chk_shift().get_active();
    if(!model_->do_fft(shift, err)){
        Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
    }
    view_.refresh();
}

void AppController::on_ifft(){
    std::string err;
    if(!model_->do_ifft(err)){
        Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run();
    }
    view_.refresh();
}
