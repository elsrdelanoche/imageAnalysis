#include "ImageModel.hpp"
#include "../utils/ImgUtils.hpp"
#include "../utils/FFT.hpp"
#include <iostream>

using Glib::RefPtr;
using cd = std::complex<double>;

bool ImageModel::load_image(const std::string& path, std::string& err){
    try{
        auto p = Gdk::Pixbuf::create_from_file(path);
        // Ensure square pow2 via scaling
        img_in_ = ImgUtils::ToSquarePow2(p, N_);
        img_result_.reset();
        img_mag_.reset();
        img_phase_.reset();
        F_.clear();
        return true;
    } catch(const Glib::FileError& e){ err = e.what(); }
      catch(const Gdk::PixbufError& e){ err = e.what(); }
      catch(const std::exception& e){ err = e.what(); }
    return false;
}

bool ImageModel::do_fft(bool shift, std::string& err){
    if(!img_in_){ err="Carga primero una imagen."; return false; }
    int N = img_in_->get_width();
    N_ = N;
    // To grayscale double
    auto g = ImgUtils::PixbufToGray(img_in_);
    // Promote to complex
    F_.assign(N*N, cd(0,0));
    for(int i=0;i<N*N;++i) F_[i] = cd(g[i], 0.0);

    // 2D FFT
    FFT::fft2d(F_, N, +1);

    // Visualizations
    if(shift) FFT::fftshift2d(F_, N);
    std::vector<double> mag, phase;
    FFT::mag_phase(F_, N, mag, phase);
    img_mag_ = ImgUtils::MagToPixbuf(mag, N);
    img_phase_ = ImgUtils::PhaseToPixbuf(phase, N);
    img_result_.reset();
    return true;
}

bool ImageModel::do_ifft(std::string& err){
    if(F_.empty()){ err="No hay espectro FFT disponible. Ejecuta primero FFT."; return false; }
    int N = N_;
    // If magnitude was shifted for display, assume F_ está en el mismo estado (sin deshacer shift)
    // Para IFFT correcta, no es necesario deshacer shift si no se modificó F_. (Solo era visualización.)
    FFT::fft2d(F_, N, -1); // IFFT
    // Extrae parte real normalizada
    std::vector<double> rec(N*N);
    for(int i=0;i<N*N;++i){
        double v = F_[i].real();
        rec[i] = std::clamp(v, 0.0, 1.0);
    }
    img_result_ = ImgUtils::GrayToPixbuf(rec, N);
    return true;
}

bool ImageModel::save_result(const std::string& path, std::string& err){
    if(!img_result_){ err="No hay imagen de salida para guardar."; return false; }
    try{
        img_result_->save(path, "png");
        return true;
    } catch(const Glib::FileError& e){ err=e.what(); }
      catch(const Gdk::PixbufError& e){ err=e.what(); }
      catch(const std::exception& e){ err=e.what(); }
    return false;
}
