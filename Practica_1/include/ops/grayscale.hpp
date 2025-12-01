#pragma once
#include "../core/filter.hpp"
#include "../core/pixel_types.hpp"

namespace ops {

enum class GrayMethod { Average, BT601, BT709 };

class Grayscale final : public core::Filter {
    GrayMethod m_;
    static inline uint8_t to_gray(uint8_t r,uint8_t g,uint8_t b, GrayMethod m) {
        switch (m) {
            case GrayMethod::Average: return uint8_t((uint16_t(r)+g+b)/3u);
            case GrayMethod::BT601:   return core::clamp_u8(0.299*r + 0.587*g + 0.114*b);
            case GrayMethod::BT709:   return core::clamp_u8(0.2126*r + 0.7152*g + 0.0722*b);
        }
        return uint8_t((uint16_t(r)+g+b)/3u);
    }
public:
    explicit Grayscale(GrayMethod m) : m_(m) {}

    core::ImageBuffer apply(const core::ImageView& src) const override {
        core::ImageBuffer out(src.width(), src.height(), 3);
        if (!src.valid()) return out;

        for (int y=0;y<src.height();++y) {
            const uint8_t* srow = src.pixel(0,y);
            uint8_t* drow = out.data() + y*out.stride();
            for (int x=0;x<src.width();++x) {
                const uint8_t* s = srow + x*src.channels();
                uint8_t g = to_gray(s[0],s[1],s[2], m_);
                uint8_t* d = drow + x*3;
                d[0]=g; d[1]=g; d[2]=g;
            }
        }
        return out;
    }
};

} // namespace ops
