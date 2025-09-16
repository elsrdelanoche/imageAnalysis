#pragma once
#include <gtk/gtk.h>
#include <vector>

struct ImageGray {
    int width=0, height=0;
    std::vector<float> data; // [0..1]
};

struct ImageRGB {
    int width=0, height=0;
    ImageGray r,g,b; // each [0..1]
};

struct ImageCMY {
    int width=0, height=0;
    ImageGray c,m,y;
};

struct ImageCMYK {
    int width=0, height=0;
    ImageGray c,m,y,k;
};

struct ImageYIQ {
    int width=0, height=0;
    ImageGray y,i,q;
};

struct ImageHSV {
    int width=0, height=0;
    ImageGray h,s,v; // h normalized [0..1] (i.e., degrees/360)
};

struct ImageHSI {
    int width=0, height=0;
    ImageGray h,s,i; // h normalized [0..1]
};

ImageRGB pixbuf_to_rgb(GdkPixbuf *pix);
GdkPixbuf* gray_to_pixbuf(const ImageGray &g);
GdkPixbuf* rgb_to_pixbuf(const ImageRGB &rgb);
GdkPixbuf* tint_gray_to_pixbuf(const ImageGray &g, float rt, float gt, float bt);
GdkPixbuf* pseudocolor_gray_to_pixbuf(const ImageGray &g);
