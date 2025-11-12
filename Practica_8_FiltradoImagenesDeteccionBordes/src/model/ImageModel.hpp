#pragma once
#include <gtkmm.h>
#include <string>
#include <vector>
struct Params {
    int k_gauss=5; double sigma=1.0;
    double alpha=1.0;
    int k_canny=5; double sigma_c=1.0; double t_low=0.1; double t_high=0.2;
};
class ImageModel {
public:
    bool load_image(const std::string& path, std::string& err);
    bool save_result(const std::string& path, std::string& err);
    Glib::RefPtr<Gdk::Pixbuf> img_in() const { return img_in_; }
    Glib::RefPtr<Gdk::Pixbuf> img_out() const { return img_out_; }
    int width() const { return w_; }
    int height() const { return h_; }
    bool apply_lowpass(const std::string& name, const Params& p, std::string& err);
    bool apply_highpass(const std::string& name, const Params& p, std::string& err);
    bool apply_edge(const std::string& name, std::string& err);
    bool apply_canny(const Params& p, std::string& err);
private:
    Glib::RefPtr<Gdk::Pixbuf> img_in_, img_out_;
    int w_=0,h_=0;
    std::vector<double> gray_;
    void set_out_from_gray(const std::vector<double>& g, bool auto_scale=false);
};
