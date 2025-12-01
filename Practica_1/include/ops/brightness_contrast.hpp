#pragma once
#include "../core/filter.hpp"
#include "../core/pixel_types.hpp"

namespace ops {

class BrightnessContrast final : public core::Filter {
    double b_, c_; // brillo, contraste
    static inline uint8_t bc(uint8_t v, double b, double c) {
        return core::clamp_u8( (double(v) + b) * c );
    }
public:
    BrightnessContrast(double brightness, double contrast) : b_(brightness), c_(contrast) {}

    core::ImageBuffer apply(const core::ImageView& src) const override {
        core::ImageBuffer out(src.width(), src.height(), 3);
        if (!src.valid()) return out;

        for (int y=0;y<src.height();++y) {
            const uint8_t* srow = src.pixel(0,y);
            uint8_t* drow = out.data() + y*out.stride();
            for (int x=0;x<src.width();++x) {
                const uint8_t* s = srow + x*src.channels();
                uint8_t* d = drow + x*3;
                d[0] = bc(s[0], b_, c_);
                d[1] = bc(s[1], b_, c_);
                d[2] = bc(s[2], b_, c_);
            }
        }
        return out;
    }
};

} // namespace ops
