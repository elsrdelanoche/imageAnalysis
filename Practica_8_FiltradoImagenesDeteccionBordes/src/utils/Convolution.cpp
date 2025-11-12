#include "Convolution.hpp"
#include "ImgUtils.hpp"
#include <cmath>
#include <algorithm>
namespace Conv {
std::vector<double> Convolve(const std::vector<double>& src, int w, int h,
                             const std::vector<double>& K, int k){
    int r = k/2;
    std::vector<double> out(w*h, 0.0);
    for(int y=0;y<h;++y){
        for(int x=0;x<w;++x){
            double acc=0.0;
            for(int j=-r;j<=r;++j){
                for(int i=-r;i<=r;++i){
                    double a = ImgUtils::sample_reflect(src, w, h, x+i, y+j);
                    double b = K[(j+r)*k + (i+r)];
                    acc += a*b;
                }
            }
            out[y*w+x] = acc;
        }
    }
    return out;
}
std::vector<double> GradMag(const std::vector<double>& gx, const std::vector<double>& gy){
    std::vector<double> m(gx.size());
    for(size_t i=0;i<m.size();++i) m[i] = std::hypot(gx[i], gy[i]);
    return m;
}
std::vector<double> NonMaxSupp(const std::vector<double>& mag, const std::vector<double>& ang, int w, int h){
    std::vector<double> out(w*h, 0.0);
    for(int y=1;y<h-1;++y){
        for(int x=1;x<w-1;++x){
            double a = ang[y*w+x];
            double deg = a * 180.0 / M_PI;
            while(deg<0) deg+=180;
            while(deg>=180) deg-=180;
            double m0 = mag[y*w+x], m1=0, m2=0;
            if((deg<22.5) || (deg>=157.5)){ m1=mag[y*w+x-1]; m2=mag[y*w+x+1]; }
            else if(deg<67.5){ m1=mag[(y-1)*w+x+1]; m2=mag[(y+1)*w+x-1]; }
            else if(deg<112.5){ m1=mag[(y-1)*w+x]; m2=mag[(y+1)*w+x]; }
            else { m1=mag[(y-1)*w+x-1]; m2=mag[(y+1)*w+x+1]; }
            out[y*w+x] = (m0>=m1 && m0>=m2) ? m0 : 0.0;
        }
    }
    return out;
}
}
