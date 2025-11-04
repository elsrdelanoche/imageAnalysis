#include "ImageModel.hpp"
#include "../utils/BinaryOps.hpp"
#include <iostream>

using Glib::RefPtr;

static RefPtr<Gdk::Pixbuf> load_pix(const std::string& path, std::string& err) {
    try {
        return Gdk::Pixbuf::create_from_file(path);
    } catch(const Glib::FileError& e) {
        err = "FileError: " + std::string(e.what());
    } catch(const Gdk::PixbufError& e) {
        err = "PixbufError: " + std::string(e.what());
    } catch(const std::exception& e) {
        err = e.what();
    }
    return RefPtr<Gdk::Pixbuf>();
}

bool ImageModel::load_image_a(const std::string& path, std::string& err) {
    auto p = load_pix(path, err);
    if(!p) return false;
    pix_a_ = p;
    clear_result();
    return true;
}
bool ImageModel::load_image_b(const std::string& path, std::string& err) {
    auto p = load_pix(path, err);
    if(!p) return false;
    pix_b_ = p;
    clear_result();
    return true;
}

RefPtr<Gdk::Pixbuf> ImageModel::ensure_gray8(const RefPtr<Gdk::Pixbuf>& src) {
    if(!src) return RefPtr<Gdk::Pixbuf>();
    if(src->get_n_channels() == 1 && !src->get_has_alpha()) {
        return src;
    }
    // convert RGB(A) to grayscale (luma)
    auto gray = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, src->get_width(), src->get_height());
    src->copy_area(0,0,src->get_width(), src->get_height(), gray, 0,0);
    // now gray is still RGB; convert to 1 channel buffer
    int w = gray->get_width(), h = gray->get_height();
    int rs = gray->get_rowstride();
    int nchan = gray->get_n_channels(); // 3
    auto out = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h);
    // create final mono buffer
    auto mono = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h); // we'll pack into 1 channel at the end by copying data
    // We cannot create 1-channel Pixbuf directly, so we'll synthesize by duplicating gray into R=G=B and then later treat as gray.
    // Simpler: allocate raw buffer and create_from_data (keep data alive)
    std::vector<unsigned char> buf(w*h);
    for(int y=0; y<h; ++y) {
        const unsigned char* p = gray->get_pixels() + y*rs;
        for(int x=0; x<w; ++x) {
            unsigned char r = p[x*nchan+0];
            unsigned char g = p[x*nchan+1];
            unsigned char b = p[x*nchan+2];
            unsigned char lum = static_cast<unsigned char>(0.299*r + 0.587*g + 0.114*b);
            buf[y*w+x] = lum;
        }
    }
    // Create 1-channel Pixbuf using create_from_data with colorspace RGB=false? Pixbuf doesn't support 1 channel directly.
    // Workaround: keep grayscale as RGB by replicating channel (R=G=B). Operations will read R only.
    auto rgb = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h);
    int rs2 = rgb->get_rowstride();
    int nc2 = rgb->get_n_channels(); // 3
    for(int y=0; y<h; ++y){
        unsigned char* p2 = rgb->get_pixels() + y*rs2;
        for(int x=0; x<w; ++x){
            unsigned char v = buf[y*w+x];
            p2[x*nc2+0] = v;
            p2[x*nc2+1] = v;
            p2[x*nc2+2] = v;
        }
    }
    return rgb;
}

RefPtr<Gdk::Pixbuf> ImageModel::binarize_128(const RefPtr<Gdk::Pixbuf>& src) {
    if(!src) return RefPtr<Gdk::Pixbuf>();
    auto g = ensure_gray8(src);
    int w = g->get_width(), h = g->get_height();
    auto out = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h);
    int rs_in = g->get_rowstride();
    int rs_out = out->get_rowstride();
    int nc = g->get_n_channels(); // 3
    int nco= out->get_n_channels(); // 3
    for(int y=0;y<h;++y){
        const unsigned char* pi = g->get_pixels() + y*rs_in;
        unsigned char* po = out->get_pixels() + y*rs_out;
        for(int x=0;x<w;++x){
            unsigned char lum = pi[x*nc+0];
            unsigned char v = (lum >= 128) ? 255 : 0;
            po[x*nco+0]=po[x*nco+1]=po[x*nco+2]=v;
        }
    }
    return out;
}

RefPtr<Gdk::Pixbuf> ImageModel::resize_to_match(const RefPtr<Gdk::Pixbuf>& a, const RefPtr<Gdk::Pixbuf>& b) {
    if(!a || !b) return RefPtr<Gdk::Pixbuf>();
    if(a->get_width()==b->get_width() && a->get_height()==b->get_height()) return b;
    return b->scale_simple(a->get_width(), a->get_height(), Gdk::INTERP_NEAREST);
}

bool ImageModel::apply_logical(LogicalOp op, bool not_a, bool not_b, std::string& err) {
    if(!pix_a_ || !pix_b_) { err = "Carga A y B primero."; return false; }
    auto A = force_binarize_ ? binarize_128(pix_a_) : ensure_gray8(pix_a_);
    auto Btmp = force_binarize_ ? binarize_128(pix_b_) : ensure_gray8(pix_b_);
    auto B = resize_to_match(A, Btmp);
    if(!A || !B) { err = "Error preparando imágenes."; return false; }
    pix_result_ = BinaryOps::logical(A, B, op, not_a, not_b);
    return static_cast<bool>(pix_result_);
}

bool ImageModel::apply_relational(RelOp op, std::string& err) {
    if(!pix_a_ || !pix_b_) { err = "Carga A y B primero."; return false; }
    auto A = force_binarize_ ? binarize_128(pix_a_) : ensure_gray8(pix_a_);
    auto Btmp = force_binarize_ ? binarize_128(pix_b_) : ensure_gray8(pix_b_);
    auto B = resize_to_match(A, Btmp);
    if(!A || !B) { err = "Error preparando imágenes."; return false; }
    pix_result_ = BinaryOps::relational(A, B, op);
    return static_cast<bool>(pix_result_);
}

bool ImageModel::save_result(const std::string& path, std::string& err) {
    if(!pix_result_) { err = "No hay resultado para guardar."; return false; }
    try {
        pix_result_->save(path, "png");
        return true;
    } catch(const Glib::FileError& e) {
        err = "FileError: " + std::string(e.what());
    } catch(const Gdk::PixbufError& e) {
        err = "PixbufError: " + std::string(e.what());
    } catch(const std::exception& e) {
        err = e.what();
    }
    return false;
}
