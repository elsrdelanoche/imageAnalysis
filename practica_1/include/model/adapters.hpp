#pragma once
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "../core/image_buffer.hpp"
#include "../core/image_view.hpp"

// Adaptadores entre GdkPixbuf (GTK) y core::ImageBuffer/ImageView
namespace model {

// Copia desde GdkPixbuf (RGB o RGBA) a ImageBuffer RGB
inline bool from_pixbuf(GdkPixbuf* px, core::ImageBuffer& out) {
    if (!px) return false;
    const int w = gdk_pixbuf_get_width(px);
    const int h = gdk_pixbuf_get_height(px);
    const int ch = gdk_pixbuf_get_n_channels(px);
    const int stride = gdk_pixbuf_get_rowstride(px);
    const uint8_t* data = gdk_pixbuf_get_pixels(px);
    if (ch < 3) return false;

    out.reset(w,h,3);
    for (int y=0;y<h;++y) {
        const uint8_t* srow = data + y*stride;
        uint8_t* drow = out.data() + y*out.stride();
        for (int x=0;x<w;++x) {
            const uint8_t* s = srow + x*ch;
            uint8_t* d = drow + x*3;
            d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; // ignorar alfa
        }
    }
    return true;
}

// Crea GdkPixbuf desde ImageView (RGB). Caller: g_object_unref().
inline GdkPixbuf* to_pixbuf(const core::ImageView& v) {
    if (!v.valid()) return nullptr;
    GdkPixbuf* px = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, v.width(), v.height());
    auto* dst = gdk_pixbuf_get_pixels(px);
    int dst_rs = gdk_pixbuf_get_rowstride(px);

    for (int y=0;y<v.height();++y) {
        const uint8_t* srow = v.pixel(0,y);
        uint8_t* drow = dst + y*dst_rs;
        std::copy(srow, srow + v.width()*3, drow);
    }
    return px;
}

} // namespace model
