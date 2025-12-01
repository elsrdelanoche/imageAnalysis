#pragma once
#include <gtkmm.h>
#include <vector>
namespace ImgUtils {
std::vector<double> PixbufToGray(const Glib::RefPtr<Gdk::Pixbuf>& p);
Glib::RefPtr<Gdk::Pixbuf> GrayToPixbuf(const std::vector<double>& g, int w, int h);
inline double clamp01(double v){ return v<0?0:(v>1?1:v); }
double sample_reflect(const std::vector<double>& img, int w, int h, int x, int y);
}
