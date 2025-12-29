#include "Canny.hpp"
#include "Convolution.hpp"
#include "Kernels.hpp"
#include <cmath>
#include <algorithm>
namespace Canny {
static void sobel(const std::vector<double>& g, int w, int h,
                  std::vector<double>& gx, std::vector<double>& gy){
    std::vector<double> Kx = {-1,0,1,-2,0,2,-1,0,1};
    std::vector<double> Ky = {-1,-2,-1,0,0,0,1,2,1};
    gx = Conv::Convolve(g,w,h,Kx,3);
    gy = Conv::Convolve(g,w,h,Ky,3);
}
std::vector<double> Run(const std::vector<double>& img, int w, int h, const Params& p){
    auto G = Kern::Gaussian(p.k_gauss, p.sigma);
    auto g = Conv::Convolve(img,w,h,G,p.k_gauss);
    std::vector<double> gx, gy; sobel(g,w,h,gx,gy);
    auto mag = Conv::GradMag(gx,gy);
    std::vector<double> ang(w*h); for(int i=0;i<w*h;++i) ang[i]=std::atan2(gy[i],gx[i]);
    auto nms = Conv::NonMaxSupp(mag,ang,w,h);

    // Normaliza magnitud a [0,1] para que T_low / T_high sean intuitivos.
    double mx = 0.0;
    for(double v: nms) mx = std::max(mx, v);
    if(mx < 1e-12) mx = 1.0;

    std::vector<unsigned char> strong(w*h,0), weak(w*h,0);
    for(int i=0;i<w*h;++i){
        double v = nms[i] / mx;
        if(v>=p.t_high) strong[i]=1;
        else if(v>=p.t_low) weak[i]=1;
    }

    // Histéresis completa: traza componentes conectadas (8-vecinos) desde píxeles fuertes.
    std::vector<unsigned char> keep(w*h,0);
    std::vector<int> stack;
    stack.reserve(w*h/8);
    for(int i=0;i<w*h;++i){
        if(strong[i]){ keep[i]=1; stack.push_back(i); }
    }
    auto inside=[&](int x,int y){ return x>=0 && y>=0 && x<w && y<h; };
    while(!stack.empty()){
        int cur = stack.back(); stack.pop_back();
        int x = cur % w, y = cur / w;
        for(int j=-1;j<=1;++j){
            for(int i=-1;i<=1;++i){
                if(i==0 && j==0) continue;
                int nx=x+i, ny=y+j;
                if(!inside(nx,ny)) continue;
                int ni = ny*w + nx;
                if(keep[ni]) continue;
                if(weak[ni]){ keep[ni]=1; stack.push_back(ni); }
            }
        }
    }

    std::vector<double> edges(w*h,0.0);
    for(int i=0;i<w*h;++i) edges[i] = keep[i] ? 1.0 : 0.0;
    return edges;
}
}
