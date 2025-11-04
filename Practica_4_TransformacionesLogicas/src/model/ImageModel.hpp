#pragma once
#include <gtkmm.h>
#include <optional>
#include <string>
#include "../utils/Types.hpp"

class ImageModel {
public:
    void set_force_binarize(bool v) { force_binarize_ = v; }
    bool force_binarize() const { return force_binarize_; }

    bool load_image_a(const std::string& path, std::string& err);
    bool load_image_b(const std::string& path, std::string& err);

    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_a() const { return pix_a_; }
    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_b() const { return pix_b_; }
    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_result() const { return pix_result_; }

    void clear_result() { pix_result_.reset(); }

    // logical apply (optionally NOT A / NOT B before op)
    bool apply_logical(LogicalOp op, bool not_a, bool not_b, std::string& err);

    // relational apply
    bool apply_relational(RelOp op, std::string& err);

    // save result
    bool save_result(const std::string& path, std::string& err);

private:
    Glib::RefPtr<Gdk::Pixbuf> pix_a_;
    Glib::RefPtr<Gdk::Pixbuf> pix_b_;
    Glib::RefPtr<Gdk::Pixbuf> pix_result_;
    bool force_binarize_ = true;

    static Glib::RefPtr<Gdk::Pixbuf> ensure_gray8(const Glib::RefPtr<Gdk::Pixbuf>& src);
    static Glib::RefPtr<Gdk::Pixbuf> binarize_128(const Glib::RefPtr<Gdk::Pixbuf>& src);
    static Glib::RefPtr<Gdk::Pixbuf> resize_to_match(const Glib::RefPtr<Gdk::Pixbuf>& a, const Glib::RefPtr<Gdk::Pixbuf>& b);
};
