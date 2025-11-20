#pragma once
#include <gtkmm.h>

namespace Noise {
Glib::RefPtr<Gdk::Pixbuf> AddSalt(const Glib::RefPtr<Gdk::Pixbuf>& src, double prob);
Glib::RefPtr<Gdk::Pixbuf> AddPepper(const Glib::RefPtr<Gdk::Pixbuf>& src, double prob);
}
