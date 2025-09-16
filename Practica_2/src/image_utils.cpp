#include "image_utils.hpp"
#include <algorithm>
#include <cmath>

static inline float clamp01(float x){ return x<0.f?0.f:(x>1.f?1.f:x); }

ImageRGB pixbuf_to_rgb(GdkPixbuf *pix) {
    ImageRGB out;
    int w = gdk_pixbuf_get_width(pix);
    int h = gdk_pixbuf_get_height(pix);
    int rs = gdk_pixbuf_get_rowstride(pix);
    int ch = gdk_pixbuf_get_n_channels(pix);
    guchar *pixels = gdk_pixbuf_get_pixels(pix);

    out.width = w; out.height = h;
    out.r.width = out.g.width = out.b.width = w;
    out.r.height = out.g.height = out.b.height = h;
    out.r.data.resize(w*h);
    out.g.data.resize(w*h);
    out.b.data.resize(w*h);

    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            guchar *p = pixels + y*rs + x*ch;
            float R = p[0]/255.f;
            float G = p[1]/255.f;
            float B = p[2]/255.f;
            int idx = y*w + x;
            out.r.data[idx]=R;
            out.g.data[idx]=G;
            out.b.data[idx]=B;
        }
    }
    return out;
}

GdkPixbuf* gray_to_pixbuf(const ImageGray &g) {
    int w=g.width, h=g.height;
    GdkPixbuf *pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
    int rs = gdk_pixbuf_get_rowstride(pix);
    int ch = gdk_pixbuf_get_n_channels(pix);
    guchar *pixels = gdk_pixbuf_get_pixels(pix);
    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            int idx = y*w + x;
            guchar v = (guchar)std::round(clamp01(g.data[idx]) * 255.0f);
            guchar *p = pixels + y*rs + x*ch;
            p[0]=v; p[1]=v; p[2]=v;
        }
    }
    return pix;
}

GdkPixbuf* rgb_to_pixbuf(const ImageRGB &rgb) {
    int w = rgb.width, h = rgb.height;
    GdkPixbuf *pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
    int rs = gdk_pixbuf_get_rowstride(pix);
    int ch = gdk_pixbuf_get_n_channels(pix);
    guchar *pixels = gdk_pixbuf_get_pixels(pix);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = y*w + x;
            guchar r = (guchar)std::round(clamp01(rgb.r.data[idx]) * 255.0f);
            guchar g = (guchar)std::round(clamp01(rgb.g.data[idx]) * 255.0f);
            guchar b = (guchar)std::round(clamp01(rgb.b.data[idx]) * 255.0f);
            guchar *p = pixels + y*rs + x*ch;
            p[0]=r; p[1]=g; p[2]=b;
        }
    }
    return pix;
}

GdkPixbuf* tint_gray_to_pixbuf(const ImageGray &g, float rt, float gt, float bt) {
    int w=g.width, h=g.height;
    GdkPixbuf *pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
    int rs = gdk_pixbuf_get_rowstride(pix);
    int ch = gdk_pixbuf_get_n_channels(pix);
    guchar *pixels = gdk_pixbuf_get_pixels(pix);
    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            int idx = y*w + x;
            float v = clamp01(g.data[idx]) * 255.0f;
            guchar r = (guchar)std::round(std::min(255.0f, v * clamp01(rt)));
            guchar gg = (guchar)std::round(std::min(255.0f, v * clamp01(gt)));
            guchar b = (guchar)std::round(std::min(255.0f, v * clamp01(bt)));
            guchar *p = pixels + y*rs + x*ch;
            p[0]=r; p[1]=gg; p[2]=b;
        }
    }
    return pix;
}

static inline void pseudo_map(float v, float &r, float &g, float &b){
    // Simple blue->cyan->green->yellow->red map
    v = clamp01(v);
    if (v < 0.25f){ // blue->cyan
        float t = v/0.25f; r = 0.f; g = t*0.75f; b = 1.f;
    } else if (v < 0.50f){ // cyan->green
        float t = (v-0.25f)/0.25f; r = 0.f; g = 0.75f + t*0.25f; b = 1.f - t;
    } else if (v < 0.75f){ // green->yellow
        float t = (v-0.50f)/0.25f; r = t; g = 1.f; b = 0.f;
    } else { // yellow->red
        float t = (v-0.75f)/0.25f; r = 1.f; g = 1.f - t; b = 0.f;
    }
}

GdkPixbuf* pseudocolor_gray_to_pixbuf(const ImageGray &g) {
    int w=g.width, h=g.height;
    GdkPixbuf *pix = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
    int rs = gdk_pixbuf_get_rowstride(pix);
    int ch = gdk_pixbuf_get_n_channels(pix);
    guchar *pixels = gdk_pixbuf_get_pixels(pix);
    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            int idx = y*w + x;
            float r,gg,b;
            pseudo_map(g.data[idx], r, gg, b);
            guchar rr = (guchar)std::round(clamp01(r) * 255.0f);
            guchar gg8 = (guchar)std::round(clamp01(gg) * 255.0f);
            guchar bb = (guchar)std::round(clamp01(b) * 255.0f);
            guchar *p = pixels + y*rs + x*ch;
            p[0]=rr; p[1]=gg8; p[2]=bb;
        }
    }
    return pix;
}
