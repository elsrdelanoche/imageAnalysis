#pragma once
#include <gtkmm.h>
#include <vector>
#include "SEFactory.hpp"

namespace MorphOps {
Glib::RefPtr<Gdk::Pixbuf> Erode(const Glib::RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se);
Glib::RefPtr<Gdk::Pixbuf> Dilate(const Glib::RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se);
Glib::RefPtr<Gdk::Pixbuf> Open (const Glib::RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se);
Glib::RefPtr<Gdk::Pixbuf> Close(const Glib::RefPtr<Gdk::Pixbuf>& src, const std::vector<unsigned char>& se);
}
