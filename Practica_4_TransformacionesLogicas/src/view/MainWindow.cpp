#include "MainWindow.hpp"
#include <iostream>

static std::string find_ui_glade() {
    // Busca assets/ui.glade en CWD y hacia arriba
    std::vector<std::string> rels;
    rels.push_back("assets/ui.glade");
    std::string prefix;
    for(int i=0;i<5;++i){
        rels.push_back(prefix + "assets/ui.glade");
        prefix += "../";
    }
    for(const auto& p : rels){
        if(Glib::file_test(p, Glib::FILE_TEST_EXISTS)) return p;
    }
    // Como último recurso, intenta al lado del ejecutable (Linux)
#ifdef __linux__
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if(len > 0){
        buf[len] = '\0';
        std::string exe = buf;
        auto pos = exe.find_last_of('/');
        if(pos != std::string::npos){
            std::string dir = exe.substr(0,pos+1);
            std::string cand = dir + "assets/ui.glade";
            if(Glib::file_test(cand, Glib::FILE_TEST_EXISTS)) return cand;
        }
    }
#endif
    return "assets/ui.glade"; // dejar un path por defecto para que el error sea claro
}

MainWindow::MainWindow(std::shared_ptr<ImageModel> model) : model_(std::move(model)) {
    set_title("Imágenes Lógicas (GTK3 · C++)");
    set_default_size(1100, 680);

    try{
        builder_ = Gtk::Builder::create_from_file(find_ui_glade());
    } catch(const Glib::FileError& e){
        std::cerr << "FileError: " << e.what() << std::endl;
        throw;
    } catch(const Glib::MarkupError& e){
        std::cerr << "MarkupError: " << e.what() << std::endl;
        throw;
    } catch(const Gtk::BuilderError& e){
        std::cerr << "BuilderError: " << e.what() << std::endl;
        throw;
    }

    Gtk::Box* root = nullptr;
    builder_->get_widget("root_box", root);
    if(!root) throw std::runtime_error("No se encontró root_box en ui.glade");
    add(*root);

    builder_->get_widget("btn_load_a", btn_load_a_);
    builder_->get_widget("btn_load_b", btn_load_b_);
    builder_->get_widget("btn_apply",  btn_apply_);
    builder_->get_widget("btn_save",   btn_save_);

    builder_->get_widget("chk_binarize", chk_binarize_);
    builder_->get_widget("chk_not_a", chk_not_a_);
    builder_->get_widget("chk_not_b", chk_not_b_);

    builder_->get_widget("img_a", img_a_);
    builder_->get_widget("img_b", img_b_);
    builder_->get_widget("img_result", img_result_);

    builder_->get_widget("rb_and", rb_and_);
    builder_->get_widget("rb_or",  rb_or_);
    builder_->get_widget("rb_xor", rb_xor_);
    builder_->get_widget("rb_eq",  rb_eq_);
    builder_->get_widget("rb_ne",  rb_ne_);
    builder_->get_widget("rb_gt",  rb_gt_);
    builder_->get_widget("rb_ge",  rb_ge_);
    builder_->get_widget("rb_lt",  rb_lt_);
    builder_->get_widget("rb_le",  rb_le_);

    show_all_children();
}

void MainWindow::refresh_images(){
    auto A = model_->get_pixbuf_a();
    auto B = model_->get_pixbuf_b();
    auto R = model_->get_pixbuf_result();
    if(img_a_) img_a_->set(A);
    if(img_b_) img_b_->set(B);
    if(img_result_) img_result_->set(R);
}
