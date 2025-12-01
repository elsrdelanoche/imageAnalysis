#pragma once
#include <algorithm>   // std::min, std::max
#include <cmath>       // std::floor
#include <cstdint>
#include "../core/filter.hpp"

namespace ops {

// Reescalado bilineal sencillo (sin alfa)
class Resize final : public core::Filter {
    int nw_, nh_;
public:
    Resize(int new_w, int new_h) : nw_(new_w), nh_(new_h) {}

    core::ImageBuffer apply(const core::ImageView& src) const override {
        core::ImageBuffer out(nw_, nh_, 3);
        if (!src.valid() || nw_ <= 0 || nh_ <= 0) return out;

        const double sx = static_cast<double>(src.width())  / static_cast<double>(nw_);
        const double sy = static_cast<double>(src.height()) / static_cast<double>(nh_);

        for (int y = 0; y < nh_; ++y) {
            const double fy = (static_cast<double>(y) + 0.5) * sy - 0.5;
            int y0 = static_cast<int>(std::floor(fy));
            y0 = std::max(0, y0);
            int y1 = std::min(y0 + 1, src.height() - 1);
            const double ty = fy - static_cast<double>(y0);

            uint8_t* drow = out.data() + y * out.stride();

            for (int x = 0; x < nw_; ++x) {
                const double fx = (static_cast<double>(x) + 0.5) * sx - 0.5;
                int x0 = static_cast<int>(std::floor(fx));
                x0 = std::max(0, x0);
                int x1 = std::min(x0 + 1, src.width() - 1);
                const double tx = fx - static_cast<double>(x0);

                const uint8_t* p00 = src.pixel(x0, y0);
                const uint8_t* p10 = src.pixel(x1, y0);
                const uint8_t* p01 = src.pixel(x0, y1);
                const uint8_t* p11 = src.pixel(x1, y1);

                uint8_t* d = drow + x * 3;
                for (int c = 0; c < 3; ++c) {
                    const double a = (1.0 - tx) * static_cast<double>(p00[c]) + tx * static_cast<double>(p10[c]);
                    const double b = (1.0 - tx) * static_cast<double>(p01[c]) + tx * static_cast<double>(p11[c]);
                    const double v = (1.0 - ty) * a + ty * b;
                    d[c] = static_cast<uint8_t>(v + 0.5);
                }
            }
        }
        return out;
    }
};

} // namespace ops
