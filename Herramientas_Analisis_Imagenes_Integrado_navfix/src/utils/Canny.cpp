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
    std::vector<unsigned char> strong(w*h,0), weak(w*h,0);
    for(int i=0;i<w*h;++i){ double v=nms[i]; if(v>=p.t_high) strong[i]=1; else if(v>=p.t_low) weak[i]=1; }
    std::vector<double> edges(w*h,0.0);
    auto idx=[&](int x,int y){return y*w+x;};
    for(int y=1;y<h-1;++y){
        for(int x=1;x<w-1;++x){
            if(strong[idx(x,y)]){ edges[idx(x,y)]=1.0; continue; }
            if(weak[idx(x,y)]){
                bool conn=false;
                for(int j=-1;j<=1 && !conn;++j)
                    for(int i=-1;i<=1 && !conn;++i)
                        if(strong[idx(x+i,y+j)]) conn=true;
                edges[idx(x,y)] = conn?1.0:0.0;
            }
        }
    }
    return edges;
}
}
