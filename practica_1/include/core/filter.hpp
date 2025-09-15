#pragma once
#include "image_view.hpp"
#include "image_buffer.hpp"

namespace core {

class Filter {
public:
    virtual ~Filter() = default;
    // Aplica filtro: entrada como vista, salida como buffer nuevo
    virtual ImageBuffer apply(const ImageView& src) const = 0;
};

} // namespace core
