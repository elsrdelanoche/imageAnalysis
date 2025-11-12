#include "ImageModel.hpp"
#include "../utils/ImgUtils.hpp"
#include "../utils/Convolution.hpp"
#include "../utils/Kernels.hpp"
#include "../utils/Canny.hpp"
#include <iostream>
#include <cmath>
using Glib::RefPtr;
bool ImageModel::load_image(const std::string& path, std::string& err){
    try{
        auto p = Gdk::Pixbuf::create_from_file(path);
        img_in_ = p; w_=p->get_width(); h_=p->get_height();
        gray_ = ImgUtils::PixbufToGray(p); img_out_.reset(); return true;
    } catch(const Glib::Error& e){ err=e.what(); return false; }
}
void ImageModel::set_out_from_gray(const std::vector<double>& g, bool auto_scale){
    std::vector<double> v=g;
    if(auto_scale){
        double mn=1e9,mx=-1e9; for(double x: v){ mn=std::min(mn,x); mx=std::max(mx,x); }
        double d=(mx-mn); if(d<=1e-9) d=1.0; for(double& x: v) x=(x-mn)/d;
    }
    img_out_ = ImgUtils::GrayToPixbuf(v, w_, h_);
}
bool ImageModel::save_result(const std::string& path, std::string& err){
    if(!img_out_){ err="No hay resultado para guardar."; return false; }
    try{ img_out_->save(path, "png"); return true; } catch(const Glib::Error& e){ err=e.what(); return false; }
}
bool ImageModel::apply_lowpass(const std::string& name, const Params& p, std::string& err){
    auto LP = Kern::LowPassKernels(p.k_gauss, p.sigma);
    for(auto& k : LP) if(k.name==name){ auto out=Conv::Convolve(gray_,w_,h_,k.data,k.k); set_out_from_gray(out,false); return true; }
    err="Kernel LP no encontrado."; return false;
}
bool ImageModel::apply_highpass(const std::string& name, const Params& p, std::string& err){
    auto HP = Kern::HighPassKernels(p.alpha);
    for(auto& k : HP) if(k.name==name){ auto out=Conv::Convolve(gray_,w_,h_,k.data,k.k); set_out_from_gray(out,true); return true; }
    if(name=="High-boost"){
        auto G = Kern::Gaussian(p.k_gauss, p.sigma);
        auto lp = Conv::Convolve(gray_, w_, h_, G, p.k_gauss);
        std::vector<double> hb(w_*h_);
        for(size_t i=0;i<hb.size();++i) hb[i] = (1.0+p.alpha)*gray_[i] - lp[i];
        set_out_from_gray(hb,true); return true;
    }
    err="Kernel HP no encontrado."; return false;
}
bool ImageModel::apply_edge(const std::string& name, std::string& err){
    if(name=="LoG"){
        auto L=Kern::LoG(7,1.2); auto out=Conv::Convolve(gray_,w_,h_,L,7); set_out_from_gray(out,true); return true;
    }
    if(name=="Laplaciano4"){
        std::vector<double> L{0,-1,0,-1,4,-1,0,-1,0}; auto out=Conv::Convolve(gray_,w_,h_,L,3); set_out_from_gray(out,true); return true;
    }
    if(name=="Laplaciano8"){
        std::vector<double> L{-1,-1,-1,-1,8,-1,-1,-1,-1}; auto out=Conv::Convolve(gray_,w_,h_,L,3); set_out_from_gray(out,true); return true;
    }
    auto EK = Kern::EdgeKernels();
    for(auto& k: EK) if(k.name==name){ auto out=Conv::Convolve(gray_,w_,h_,k.data,k.k); set_out_from_gray(out,true); return true; }
    if(name=="Sobel (mag)"){
        std::vector<double> Kx = {-1,0,1,-2,0,2,-1,0,1};
        std::vector<double> Ky = {-1,-2,-1,0,0,0,1,2,1};
        auto gx = Conv::Convolve(gray_, w_, h_, Kx, 3);
        auto gy = Conv::Convolve(gray_, w_, h_, Ky, 3);
        auto m = Conv::GradMag(gx,gy);
        set_out_from_gray(m, true); return true;
    }
    if(name=="Prewitt (mag)"){
        std::vector<double> Kx = {-1,0,1,-1,0,1,-1,0,1};
        std::vector<double> Ky = {-1,-1,-1,0,0,0,1,1,1};
        auto gx = Conv::Convolve(gray_, w_, h_, Kx, 3);
        auto gy = Conv::Convolve(gray_, w_, h_, Ky, 3);
        auto m = Conv::GradMag(gx,gy);
        set_out_from_gray(m, true); return true;
    }
    err="Kernel de bordes no soportado."; return false;
}
bool ImageModel::apply_canny(const Params& p, std::string& err){
    auto edges = Canny::Run(gray_, w_, h_, {p.k_canny, p.sigma_c, p.t_low, p.t_high});
    set_out_from_gray(edges,false); return true;
}
