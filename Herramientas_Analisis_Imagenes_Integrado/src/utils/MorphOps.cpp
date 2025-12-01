#include "MorphOps.hpp"

using Glib::RefPtr;

static inline unsigned char getL(const RefPtr<Gdk::Pixbuf>& p, int x, int y){
    int w=p->get_width(), h=p->get_height();
    x = std::max(0,std::min(w-1,x));
    y = std::max(0,std::min(h-1,y));
    int rs=p->get_rowstride(), nc=p->get_n_channels();
    const unsigned char* px = p->get_pixels()+y*rs + x*nc;
    return px[0];
}

static RefPtr<Gdk::Pixbuf> make_like(const RefPtr<Gdk::Pixbuf>& p){
    return Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, p->get_width(), p->get_height());
}

RefPtr<Gdk::Pixbuf> MorphOps::Erode(const RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se){
    int w=src->get_width(), h=src->get_height();
    auto out = make_like(src);
    int rso = out->get_rowstride(), nco = out->get_n_channels();

    int k = (int)std::sqrt(se.size());
    int rad = k/2;
    for(int y=0;y<h;++y){
        unsigned char* po = out->get_pixels()+y*rso;
        for(int x=0;x<w;++x){
            bool ok=true;
            for(int j=-rad;j<=rad && ok;++j){
                for(int i=-rad;i<=rad;++i){
                    int idx = (j+rad)*k + (i+rad);
                    if(!se[idx]) continue;
                    if(getL(src, x+i, y+j) < 128){ ok=false; break; }
                }
            }
            unsigned char v = ok?255:0;
            po[x*nco+0]=po[x*nco+1]=po[x*nco+2]=v;
        }
    }
    return out;
}

RefPtr<Gdk::Pixbuf> MorphOps::Dilate(const RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se){
    int w=src->get_width(), h=src->get_height();
    auto out = make_like(src);
    int rso = out->get_rowstride(), nco=out->get_n_channels();

    int k=(int)std::sqrt(se.size());
    int rad=k/2;
    for(int y=0;y<h;++y){
        unsigned char* po = out->get_pixels()+y*rso;
        for(int x=0;x<w;++x){
            bool on=false;
            for(int j=-rad;j<=rad && !on;++j){
                for(int i=-rad;i<=rad;++i){
                    int idx=(j+rad)*k+(i+rad);
                    if(!se[idx]) continue;
                    if(getL(src, x+i, y+j) >= 128){ on=true; break; }
                }
            }
            unsigned char v = on?255:0;
            po[x*nco+0]=po[x*nco+1]=po[x*nco+2]=v;
        }
    }
    return out;
}

RefPtr<Gdk::Pixbuf> MorphOps::Open(const RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se){
    auto e = Erode(src, se);
    return Dilate(e, se);
}
RefPtr<Gdk::Pixbuf> MorphOps::Close(const RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se){
    auto d = Dilate(src, se);
    return Erode(d, se);
}
