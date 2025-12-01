#include "ImgUtils.hpp"
#include <algorithm>
using Glib::RefPtr;
namespace ImgUtils {
std::vector<double> PixbufToGray(const RefPtr<Gdk::Pixbuf>& p){
    int w=p->get_width(), h=p->get_height();
    int rs=p->get_rowstride(), nc=p->get_n_channels();
    std::vector<double> out(w*h);
    for(int y=0;y<h;++y){
        const unsigned char* row = p->get_pixels()+y*rs;
        for(int x=0;x<w;++x){
            const unsigned char* px = row + x*nc;
            double r=px[0], g=px[1], b=px[2];
            double v = 0.299*r + 0.587*g + 0.114*b;
            out[y*w+x] = v/255.0;
        }
    }
    return out;
}
Glib::RefPtr<Gdk::Pixbuf> GrayToPixbuf(const std::vector<double>& g, int w, int h){
    auto out = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, w, h);
    int rs=out->get_rowstride(), nc=out->get_n_channels();
    for(int y=0;y<h;++y){
        unsigned char* row = out->get_pixels()+y*rs;
        for(int x=0;x<w;++x){
            unsigned char u = (unsigned char)std::round(clamp01(g[y*w+x])*255.0);
            row[x*nc+0]=row[x*nc+1]=row[x*nc+2]=u;
        }
    }
    return out;
}
static int reflect_index(int i, int n){
    if(i<0) return -i-1;
    if(i>=n) return 2*n - i - 1;
    return i;
}
double sample_reflect(const std::vector<double>& img, int w, int h, int x, int y){
    int xx = reflect_index(x, w);
    int yy = reflect_index(y, h);
    return img[yy*w+xx];
}
}
