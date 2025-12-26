#include "AppController.hpp"

#include <vector>
#include <string>
#include <algorithm>

static void cb_fill(Gtk::ComboBoxText& cb, const std::vector<std::string>& items){
    cb.remove_all();
    for(const auto& s: items) cb.append(s);
    if(!items.empty()) cb.set_active(0);
}

AppController::AppController(std::shared_ptr<ImageModel> model, MainWindow& view)
: model_(std::move(model)), view_(view){
    view_.btn_load().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_load));
    view_.btn_save().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_save));
    view_.btn_apply().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_apply));
    view_.btn_canny().signal_clicked().connect(sigc::mem_fun(*this,&AppController::on_canny));

    view_.cb_cat().signal_changed().connect(sigc::mem_fun(*this,&AppController::on_cat_changed));
    view_.cb_kernel().signal_changed().connect(sigc::mem_fun(*this,&AppController::on_kernel_changed));

    refresh_kernel_list();
    refresh_param_sensitivity();
}

void AppController::on_cat_changed(){
    refresh_kernel_list();
    refresh_param_sensitivity();
}

void AppController::on_kernel_changed(){
    refresh_param_sensitivity();
}

void AppController::refresh_kernel_list(){
    const auto cat = view_.cb_cat().get_active_text();

    if(cat=="Pasa-bajas"){
        cb_fill(view_.cb_kernel(), {"Media 3x3","Media 5x5","Media 7x7","Gauss"});
    }else if(cat=="Pasa-altas"){
        cb_fill(view_.cb_kernel(), {"Sharpen","Laplaciano4","Laplaciano8","High-boost"});
    }else if(cat=="Bordes"){
        cb_fill(view_.cb_kernel(), {
            "Roberts Gx","Roberts Gy",
            "Prewitt Gx","Prewitt Gy","Prewitt (mag)",
            "Sobel Gx","Sobel Gy","Sobel (mag)",
            "Scharr Gx","Scharr Gy",
            "Kirsch N","Kirsch E",
            "Robinson N","Robinson E",
            "Laplaciano4","Laplaciano8","LoG"
        });
    }
}

void AppController::refresh_param_sensitivity(){
    const auto cat  = view_.cb_cat().get_active_text();
    const auto name = view_.cb_kernel().get_active_text();

    // Por defecto: todo deshabilitado excepto parámetros Canny.
    view_.sp_k().set_sensitive(false);
    view_.sp_gauss_k().set_sensitive(false);
    view_.sp_sigma().set_sensitive(false);
    view_.sp_alpha().set_sensitive(false);

    // Canny siempre activo.
    view_.sp_canny_k().set_sensitive(true);
    view_.sp_sigma_c().set_sensitive(true);
    view_.sp_tlow().set_sensitive(true);
    view_.sp_thigh().set_sensitive(true);

    if(cat=="Pasa-bajas"){
        // Gauss usa k y sigma; las medias son fijas (3/5/7) por el selector.
        if(name=="Gauss"){ view_.sp_gauss_k().set_sensitive(true); view_.sp_sigma().set_sensitive(true); }
    }else if(cat=="Pasa-altas"){
        // High-boost = unsharp masking (usa Gauss + alpha)
        if(name=="High-boost"){
            view_.sp_gauss_k().set_sensitive(true);
            view_.sp_sigma().set_sensitive(true);
            view_.sp_alpha().set_sensitive(true);
        }
    }else if(cat=="Bordes"){
        // LoG configurable con k y sigma
        if(name=="LoG"){
            view_.sp_k().set_sensitive(true);
            view_.sp_sigma().set_sensitive(true);
        }
    }
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
    p.k=view_.sp_k().get_value_as_int(); if(p.k%2==0) p.k+=1;
    p.sigma=view_.sp_sigma().get_value(); p.alpha=view_.sp_alpha().get_value();
    std::string err; bool ok=false;
    if(cat=="Pasa-bajas"){ ok=model_->apply_lowpass(name,p,err); }
    else if(cat=="Pasa-altas"){ ok=model_->apply_highpass(name,p,err); }
    else if(cat=="Bordes"){ ok=model_->apply_edge(name,p,err); }
    if(!ok && !err.empty()){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
void AppController::on_canny(){
    Params p; p.k_canny=view_.sp_canny_k().get_value_as_int(); if(p.k_canny%2==0) p.k_canny+=1;
    p.sigma_c=view_.sp_sigma_c().get_value(); p.t_low=view_.sp_tlow().get_value(); p.t_high=view_.sp_thigh().get_value();
    std::string err; if(!model_->apply_canny(p,err)){ Gtk::MessageDialog m(view_, err, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true); m.run(); }
    view_.refresh();
}
