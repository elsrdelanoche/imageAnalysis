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

    // UI usa [0,1] => umbrales relativos al máximo.
    double mx = 0.0;
    for(double v: nms) mx = std::max(mx, v);
    if(mx <= 1e-12) return std::vector<double>(w*h, 0.0);

    double th = p.t_high * mx;
    double tl = p.t_low  * mx;
    if(tl > th) std::swap(tl, th);

    // 0 = none, 1 = weak, 2 = strong
    std::vector<unsigned char> cls(w*h, 0);
    for(int i=0;i<w*h;++i){
        double v = nms[i];
        if(v >= th) cls[i] = 2;
        else if(v >= tl) cls[i] = 1;
    }

    // Histéresis completa: expandir desde fuertes y "absorber" débiles conectados.
    std::vector<double> edges(w*h, 0.0);
    std::vector<int> st;
    st.reserve(w*h/8);
    for(int i=0;i<w*h;++i){
        if(cls[i] == 2){
            edges[i] = 1.0;
            st.push_back(i);
        }
    }

    auto inside = [&](int x,int y){ return (x>=0 && x<w && y>=0 && y<h); };
    while(!st.empty()){
        int cur = st.back(); st.pop_back();
        int cx = cur % w;
        int cy = cur / w;
        for(int j=-1;j<=1;++j){
            for(int i=-1;i<=1;++i){
                if(i==0 && j==0) continue;
                int nx=cx+i, ny=cy+j;
                if(!inside(nx,ny)) continue;
                int ni = ny*w + nx;
                if(cls[ni] == 1){
                    cls[ni] = 2;      // promote to strong
                    edges[ni] = 1.0;
                    st.push_back(ni);
                }
            }
        }
    }

    return edges;
}
}
