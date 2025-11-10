#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <cmath>

class ImageProcessor {
public:
    ImageProcessor() = default;
    ~ImageProcessor() = default;

    Glib::RefPtr<Gdk::Pixbuf> translate(Glib::RefPtr<Gdk::Pixbuf> src, double tx, double ty);
    Glib::RefPtr<Gdk::Pixbuf> rotate(Glib::RefPtr<Gdk::Pixbuf> src, double angle_degrees);
    Glib::RefPtr<Gdk::Pixbuf> scale(Glib::RefPtr<Gdk::Pixbuf> src, double sx, double sy, Gdk::InterpType interp_type);

private:
    guint8 getPixelValue(const guint8* pixels, int width, int height, int rowstride, 
                         int n_channels, int x, int y, int channel);
    void setPixelValue(guint8* pixels, int rowstride, int n_channels, 
                      int x, int y, int channel, guint8 value);
};

#endif
