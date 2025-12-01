#include "FFTImgUtils.hpp"
#include <algorithm>
#include <cmath>

namespace FFTImgUtils {

int NextPow2(int v){ int p=1; while(p<v) p<<=1; return p; }
bool IsPow2(int v){ return v>0 && (v&(v-1))==0; }

Glib::RefPtr<Gdk::Pixbuf> ToSquarePow2(const Glib::RefPtr<Gdk::Pixbuf>& src, int& N_out){
    int w=src->get_width(), h=src->get_height();
    int M = std::max(w,h);
    int N = NextPow2(M);
    N_out = N;
    auto scaled = src->scale_simple(N,N, Gdk::INTERP_BILINEAR);
    return scaled;
}

std::vector<double> PixbufToGray(const Glib::RefPtr<Gdk::Pixbuf>& p){
    int N = p->get_width(); // assume square
    int rs = p->get_rowstride();
    int nc = p->get_n_channels();
    std::vector<double> out(N*N, 0.0);
    for(int y=0;y<N;++y){
        const unsigned char* row = p->get_pixels() + y*rs;
        for(int x=0;x<N;++x){
            const unsigned char* px = row + x*nc;
            double r=px[0], g=px[1], b=px[2];
            double v = 0.299*r + 0.587*g + 0.114*b;
            out[y*N+x] = v/255.0;
        }
    }
    return out;
}

Glib::RefPtr<Gdk::Pixbuf> GrayToPixbuf(const std::vector<double>& g, int N){
    auto out = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, N, N);
    int rs = out->get_rowstride(), nc=out->get_n_channels();
    for(int y=0;y<N;++y){
        unsigned char* row = out->get_pixels() + y*rs;
        for(int x=0;x<N;++x){
            double v = std::clamp(g[y*N+x], 0.0, 1.0);
            unsigned char u = static_cast<unsigned char>(std::round(v*255.0));
            row[x*nc+0]=row[x*nc+1]=row[x*nc+2]=u;
        }
    }
    return out;
}

Glib::RefPtr<Gdk::Pixbuf> MagToPixbuf(const std::vector<double>& mag, int N){
    // log(1+|F|) normalization
    double mx=0.0;
    std::vector<double> lg(N*N);
    for(int i=0;i<N*N;++i){
        double v = std::log1p(std::abs(mag[i]));
        lg[i]=v; if(v>mx) mx=v;
    }
    if(mx<=0) mx=1;
    for(double& v: lg) v/=mx;
    return GrayToPixbuf(lg, N);
}

Glib::RefPtr<Gdk::Pixbuf> PhaseToPixbuf(const std::vector<double>& phase, int N){
    std::vector<double> ph(N*N);
    for(int i=0;i<N*N;++i){
        // map [-pi,pi] -> [0,1]
        ph[i] = (phase[i] + M_PI) / (2*M_PI);
    }
    return GrayToPixbuf(ph, N);
}

} // namespace
