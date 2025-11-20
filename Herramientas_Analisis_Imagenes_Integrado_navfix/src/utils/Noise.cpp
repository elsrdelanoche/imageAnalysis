#include "Noise.hpp"
#include <random>
using Glib::RefPtr;

static RefPtr<Gdk::Pixbuf> make_like(const RefPtr<Gdk::Pixbuf>& p){
    return Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, p->get_width(), p->get_height());
}

Glib::RefPtr<Gdk::Pixbuf> Noise::AddSalt(const RefPtr<Gdk::Pixbuf>& src, double prob){
    auto out = make_like(src);
    src->copy_area(0,0,src->get_width(), src->get_height(), out, 0,0);
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<> U(0.0,1.0);
    int w=out->get_width(), h=out->get_height();
    int rs=out->get_rowstride(), nc=out->get_n_channels();
    for(int y=0;y<h;++y){
        unsigned char* p = out->get_pixels()+y*rs;
        for(int x=0;x<w;++x){
            if(U(rng) < prob){
                p[x*nc+0]=p[x*nc+1]=p[x*nc+2]=255;
            }
        }
    }
    return out;
}

Glib::RefPtr<Gdk::Pixbuf> Noise::AddPepper(const RefPtr<Gdk::Pixbuf>& src, double prob){
    auto out = make_like(src);
    src->copy_area(0,0,src->get_width(), src->get_height(), out, 0,0);
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<> U(0.0,1.0);
    int w=out->get_width(), h=out->get_height();
    int rs=out->get_rowstride(), nc=out->get_n_channels();
    for(int y=0;y<h;++y){
        unsigned char* p = out->get_pixels()+y*rs;
        for(int x=0;x<w;++x){
            if(U(rng) < prob){
                p[x*nc+0]=p[x*nc+1]=p[x*nc+2]=0;
            }
        }
    }
    return out;
}
