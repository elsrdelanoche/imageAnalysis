#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "../core/pixel_types.hpp"
#include "../core/image_view.hpp"

namespace io {

enum class IntImageMode { COLOR_RGB_PACKED, RED_8U, GREEN_8U, BLUE_8U, GRAY_8U };

// Vuelca CSV (matriz 2D). Retorna false si falla apertura o src inválida.
inline bool export_csv(const std::string& path, const core::ImageView& src, IntImageMode mode) {
    if (!src.valid()) return false;
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;

    for (int y=0;y<src.height();++y) {
        const uint8_t* row = src.pixel(0,y);
        for (int x=0;x<src.width();++x) {
            const uint8_t* p = row + x*src.channels();
            uint32_t v=0;
            switch (mode) {
                case IntImageMode::COLOR_RGB_PACKED:
                    v = core::pack_rgb_0x00RRGGBB(p[0],p[1],p[2]); break;
                case IntImageMode::RED_8U:   v = p[0]; break;
                case IntImageMode::GREEN_8U: v = p[1]; break;
                case IntImageMode::BLUE_8U:  v = p[2]; break;
                case IntImageMode::GRAY_8U:  v = (uint32_t(p[0])+p[1]+p[2])/3u; break;
            }
            ofs << v << (x+1<src.width() ? ',' : '\n');
        }
    }
    return true;
}

} // namespace io
