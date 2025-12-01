
#pragma once
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "../core/image_buffer.hpp"
#include "../core/image_view.hpp"

namespace model {

inline bool from_pixbuf(GdkPixbuf* px, core::ImageBuffer& out){
    if(!px) return false;
    int w=gdk_pixbuf_get_width(px), h=gdk_pixbuf_get_height(px);
    int ch=gdk_pixbuf_get_n_channels(px), rs=gdk_pixbuf_get_rowstride(px);
    const unsigned char* src=gdk_pixbuf_get_pixels(px);
    out.reset(w,h,3);
    for(int y=0;y<h;++y){
        const unsigned char* s=src + y*rs;
        unsigned char* d=out.data() + y*out.stride();
        for(int x=0;x<w;++x){
            if (ch>=3){
                d[0]=s[0]; d[1]=s[1]; d[2]=s[2];
            }else{
                d[0]=d[1]=d[2]=s[0];
            }
            s+=ch; d+=3;
        }
    }
    return true;
}

inline GdkPixbuf* to_pixbuf(const core::ImageView& v){
    GdkPixbuf* px = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, v.width(), v.height());
    auto* dst = gdk_pixbuf_get_pixels(px);
    int rs = gdk_pixbuf_get_rowstride(px);
    for(int y=0;y<v.height();++y){
        const unsigned char* s=v.pixel(0,y);
        unsigned char* d=dst + y*rs;
        std::copy(s, s+v.width()*3, d);
    }
    return px;
}

} // namespace model
