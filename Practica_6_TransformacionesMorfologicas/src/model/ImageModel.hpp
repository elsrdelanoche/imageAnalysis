#pragma once
#include <gtkmm.h>
#include <string>

#include "../utils/Types.hpp"

class ImageModel {
public:
    void set_force_binarize(bool v){ force_binarize_ = v; }
    bool force_binarize() const { return force_binarize_; }

    bool load_image(const std::string& path, std::string& err);
    Glib::RefPtr<Gdk::Pixbuf> img_in() const { return img_in_; }
    Glib::RefPtr<Gdk::Pixbuf> img_out() const { return img_out_; }

    void clear_out(){ img_out_.reset(); }

    void set_se(SEShape shape, int size){ se_shape_=shape; se_size_=size|1; }
    SEShape se_shape() const { return se_shape_; }
    int se_size() const { return se_size_; }

    bool erode(std::string& err);
    bool dilate(std::string& err);
    bool open(std::string& err);
    bool close(std::string& err);

    bool add_salt(double prob, std::string& err);
    bool add_pepper(double prob, std::string& err);
    bool remove_salt(std::string& err);
    bool remove_pepper(std::string& err);

    bool save_out(const std::string& path, std::string& err);

private:
    Glib::RefPtr<Gdk::Pixbuf> img_in_;
    Glib::RefPtr<Gdk::Pixbuf> img_out_;
    bool force_binarize_ = true; // Python -> needs lowercase true
    SEShape se_shape_ = SEShape::Square;
    int se_size_ = 3;

    Glib::RefPtr<Gdk::Pixbuf> ensure_gray(const Glib::RefPtr<Gdk::Pixbuf>& p);
    Glib::RefPtr<Gdk::Pixbuf> binarize_128(const Glib::RefPtr<Gdk::Pixbuf>& p);
};
