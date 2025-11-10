#pragma once
#include <gtkmm.h>
#include <complex>
#include <vector>

class ImageModel {
public:
    bool load_image(const std::string& path, std::string& err);
    bool save_result(const std::string& path, std::string& err);

    Glib::RefPtr<Gdk::Pixbuf> img_in() const { return img_in_; }
    Glib::RefPtr<Gdk::Pixbuf> img_result() const { return img_result_; }
    Glib::RefPtr<Gdk::Pixbuf> img_mag() const { return img_mag_; }
    Glib::RefPtr<Gdk::Pixbuf> img_phase() const { return img_phase_; }

    bool do_fft(bool shift, std::string& err);
    bool do_ifft(std::string& err);

    int sizeN() const { return N_; }

private:
    Glib::RefPtr<Gdk::Pixbuf> img_in_, img_result_, img_mag_, img_phase_;
    int N_ = 0;

    // Frequency domain buffer
    std::vector<std::complex<double>> F_;
};
