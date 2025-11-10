#include "ImageModel.hpp"
#include "../utils/MorphOps.hpp"
#include "../utils/SEFactory.hpp"
#include "../utils/Noise.hpp"
#include <iostream>

using Glib::RefPtr;

static RefPtr<Gdk::Pixbuf> load_pix(const std::string& path, std::string& err){
    try{
        return Gdk::Pixbuf::create_from_file(path);
    } catch(const Glib::FileError& e){ err = e.what(); }
      catch(const Gdk::PixbufError& e){ err = e.what(); }
      catch(const std::exception& e){ err = e.what(); }
    return RefPtr<Gdk::Pixbuf>();
}

bool ImageModel::load_image(const std::string& path, std::string& err){
    auto p = load_pix(path, err);
    if(!p) return false;
    img_in_ = p;
    img_out_.reset();
    return true;
}

RefPtr<Gdk::Pixbuf> ImageModel::ensure_gray(const RefPtr<Gdk::Pixbuf>& p){
    if(!p) return RefPtr<Gdk::Pixbuf>();
    auto rgb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, p->get_width(), p->get_height());
    p->copy_area(0,0,p->get_width(), p->get_height(), rgb, 0,0);
    return rgb;
}

RefPtr<Gdk::Pixbuf> ImageModel::binarize_128(const RefPtr<Gdk::Pixbuf>& p){
    auto g = ensure_gray(p);
    int w=g->get_width(), h=g->get_height();
    auto out = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h);
    int rsi=g->get_rowstride(), rso=out->get_rowstride();
    int nci=g->get_n_channels(), nco=out->get_n_channels();
    for(int y=0;y<h;++y){
        const unsigned char* pi = g->get_pixels()+y*rsi;
        unsigned char* po = out->get_pixels()+y*rso;
        for(int x=0;x<w;++x){
            unsigned char v = pi[x*nci+0];
            unsigned char b = (v>=128)?255:0;
            po[x*nco+0]=po[x*nco+1]=po[x*nco+2]=b;
        }
    }
    return out;
}

bool ImageModel::erode(std::string& err){
    if(!img_in_) { err="Carga una imagen."; return false; }
    auto src = force_binarize_ ? binarize_128(img_in_) : ensure_gray(img_in_);
    auto se = SEFactory::make(se_shape_, se_size_);
    img_out_ = MorphOps::Erode(src, se);
    return (bool)img_out_;
}

bool ImageModel::dilate(std::string& err){
    if(!img_in_) { err="Carga una imagen."; return false; }
    auto src = force_binarize_ ? binarize_128(img_in_) : ensure_gray(img_in_);
    auto se = SEFactory::make(se_shape_, se_size_);
    img_out_ = MorphOps::Dilate(src, se);
    return (bool)img_out_;
}

bool ImageModel::open(std::string& err){
    if(!img_in_) { err="Carga una imagen."; return false; }
    auto src = force_binarize_ ? binarize_128(img_in_) : ensure_gray(img_in_);
    auto se = SEFactory::make(se_shape_, se_size_);
    img_out_ = MorphOps::Open(src, se);
    return (bool)img_out_;
}

bool ImageModel::close(std::string& err){
    if(!img_in_) { err="Carga una imagen."; return false; }
    auto src = force_binarize_ ? binarize_128(img_in_) : ensure_gray(img_in_);
    auto se = SEFactory::make(se_shape_, se_size_);
    img_out_ = MorphOps::Close(src, se);
    return (bool)img_out_;
}

bool ImageModel::add_salt(double prob, std::string& err){
    if(!img_in_) { err="Carga una imagen."; return false; }
    auto src = force_binarize_ ? binarize_128(img_in_) : ensure_gray(img_in_);
    img_out_ = Noise::AddSalt(src, prob);
    return (bool)img_out_;
}

bool ImageModel::add_pepper(double prob, std::string& err){
    if(!img_in_) { err="Carga una imagen."; return false; }
    auto src = force_binarize_ ? binarize_128(img_in_) : ensure_gray(img_in_);
    img_out_ = Noise::AddPepper(src, prob);
    return (bool)img_out_;
}

bool ImageModel::remove_salt(std::string& err){ return open(err); }
bool ImageModel::remove_pepper(std::string& err){ return close(err); }

bool ImageModel::save_out(const std::string& path, std::string& err){
    if(!img_out_) { err="No hay resultado."; return false; }
    try{
        img_out_->save(path, "png");
        return true;
    } catch(const Glib::FileError& e){ err=e.what(); }
      catch(const Gdk::PixbufError& e){ err=e.what(); }
      catch(const std::exception& e){ err=e.what(); }
    return false;
}
