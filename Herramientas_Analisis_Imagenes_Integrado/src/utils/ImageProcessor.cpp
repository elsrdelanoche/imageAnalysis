#include "utils/ImageProcessor.h"
#include <algorithm>

Glib::RefPtr<Gdk::Pixbuf> ImageProcessor::translate(
    Glib::RefPtr<Gdk::Pixbuf> src, double tx, double ty) {
    
    if (!src) return Glib::RefPtr<Gdk::Pixbuf>();
    
    int width = src->get_width();
    int height = src->get_height();
    int n_channels = src->get_n_channels();
    
    auto dst = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, src->get_has_alpha(), 8, width, height);
    dst->fill(0x00000000);
    
    const guint8* src_pixels = src->get_pixels();
    guint8* dst_pixels = dst->get_pixels();
    int src_rowstride = src->get_rowstride();
    int dst_rowstride = dst->get_rowstride();
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int src_x = x - static_cast<int>(tx);
            int src_y = y - static_cast<int>(ty);
            
            if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
                for (int c = 0; c < n_channels; c++) {
                    guint8 value = getPixelValue(src_pixels, width, height, src_rowstride, 
                                                 n_channels, src_x, src_y, c);
                    setPixelValue(dst_pixels, dst_rowstride, n_channels, x, y, c, value);
                }
            }
        }
    }
    
    return dst;
}

Glib::RefPtr<Gdk::Pixbuf> ImageProcessor::rotate(
    Glib::RefPtr<Gdk::Pixbuf> src, double angle_degrees) {
    
    if (!src) return Glib::RefPtr<Gdk::Pixbuf>();
    
    int src_width = src->get_width();
    int src_height = src->get_height();
    int n_channels = src->get_n_channels();
    
    double angle_rad = -angle_degrees * M_PI / 180.0;
    double cos_a = std::cos(angle_rad);
    double sin_a = std::sin(angle_rad);
    
    double corners_x[4] = {0.0, static_cast<double>(src_width), static_cast<double>(src_width), 0.0};
    double corners_y[4] = {0.0, 0.0, static_cast<double>(src_height), static_cast<double>(src_height)};
    double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    
    for (int i = 0; i < 4; i++) {
        double x = corners_x[i];
        double y = corners_y[i];
        
        double rx = x * cos_a - y * sin_a;
        double ry = x * sin_a + y * cos_a;
        
        if (i == 0) {
            min_x = max_x = rx;
            min_y = max_y = ry;
        } else {
            min_x = std::min(min_x, rx);
            max_x = std::max(max_x, rx);
            min_y = std::min(min_y, ry);
            max_y = std::max(max_y, ry);
        }
    }
    
    int dst_width = static_cast<int>(std::ceil(max_x - min_x));
    int dst_height = static_cast<int>(std::ceil(max_y - min_y));
    
    auto dst = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, src->get_has_alpha(), 8, 
                                   dst_width, dst_height);
    dst->fill(0x00000000);
    
    const guint8* src_pixels = src->get_pixels();
    guint8* dst_pixels = dst->get_pixels();
    int src_rowstride = src->get_rowstride();
    int dst_rowstride = dst->get_rowstride();
    
    double offset_x = -min_x;
    double offset_y = -min_y;
    
    for (int y = 0; y < dst_height; y++) {
        for (int x = 0; x < dst_width; x++) {
            double dx = x - offset_x;
            double dy = y - offset_y;
            
            double src_x = dx * cos_a + dy * sin_a;
            double src_y = -dx * sin_a + dy * cos_a;
            
            int ix = static_cast<int>(std::round(src_x));
            int iy = static_cast<int>(std::round(src_y));
            
            if (ix >= 0 && ix < src_width && iy >= 0 && iy < src_height) {
                for (int c = 0; c < n_channels; c++) {
                    guint8 value = getPixelValue(src_pixels, src_width, src_height, 
                                                 src_rowstride, n_channels, ix, iy, c);
                    setPixelValue(dst_pixels, dst_rowstride, n_channels, x, y, c, value);
                }
            }
        }
    }
    
    return dst;
}

Glib::RefPtr<Gdk::Pixbuf> ImageProcessor::scale(
    Glib::RefPtr<Gdk::Pixbuf> src, double sx, double sy, Gdk::InterpType interp_type) {
    
    if (!src) return Glib::RefPtr<Gdk::Pixbuf>();
    
    int src_width = src->get_width();
    int src_height = src->get_height();
    
    int dst_width = static_cast<int>(src_width * sx);
    int dst_height = static_cast<int>(src_height * sy);
    
    if (dst_width < 1) dst_width = 1;
    if (dst_height < 1) dst_height = 1;
    
    return src->scale_simple(dst_width, dst_height, interp_type);
}

guint8 ImageProcessor::getPixelValue(const guint8* pixels, int width, int height, 
                                     int rowstride, int n_channels, int x, int y, int channel) {
    if (x < 0 || x >= width || y < 0 || y >= height || channel >= n_channels) {
        return 0;
    }
    return pixels[y * rowstride + x * n_channels + channel];
}

void ImageProcessor::setPixelValue(guint8* pixels, int rowstride, int n_channels, 
                                   int x, int y, int channel, guint8 value) {
    pixels[y * rowstride + x * n_channels + channel] = value;
}
