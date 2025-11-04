#pragma once
#include <gtkmm.h>
#include <string>
#include "Types.hpp"

namespace BinaryOps {

Glib::RefPtr<Gdk::Pixbuf> logical(const Glib::RefPtr<Gdk::Pixbuf>& A,
                                  const Glib::RefPtr<Gdk::Pixbuf>& B,
                                  LogicalOp op,
                                  bool not_a,
                                  bool not_b);

Glib::RefPtr<Gdk::Pixbuf> relational(const Glib::RefPtr<Gdk::Pixbuf>& A,
                                     const Glib::RefPtr<Gdk::Pixbuf>& B,
                                     RelOp op);

}
