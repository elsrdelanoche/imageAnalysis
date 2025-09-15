#pragma once
#include <string>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "../core/image_buffer.hpp"
#include "../model/adapters.hpp"

namespace io {

// Carga archivo a GdkPixbuf y lo adapta a core::ImageBuffer (RGB)
inline bool load_image_to_buffer(const std::string& path, core::ImageBuffer& out) {
    GError* err=nullptr;
    GdkPixbuf* px = gdk_pixbuf_new_from_file(path.c_str(), &err);
    if (err) { g_error_free(err); return false; }
    bool ok = model::from_pixbuf(px, out); // copia a RGB
    g_object_unref(px);
    return ok;
}

} // namespace io
