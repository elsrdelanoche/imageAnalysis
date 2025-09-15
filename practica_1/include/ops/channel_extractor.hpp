#pragma once
#include "../core/filter.hpp"

namespace ops {

enum class Channel { R, G, B };

// Devuelve imagen “pintada” sólo con el canal elegido (otros en 0)
class ChannelExtractor final : public core::Filter {
    Channel ch_;
public:
    explicit ChannelExtractor(Channel ch) : ch_(ch) {}

    core::ImageBuffer apply(const core::ImageView& src) const override {
        core::ImageBuffer out(src.width(), src.height(), 3);
        if (!src.valid()) return out;

        for (int y=0;y<src.height();++y) {
            const uint8_t* srow = src.pixel(0,y);
            uint8_t* drow = out.data() + y*out.stride();
            for (int x=0;x<src.width();++x) {
                const uint8_t* s = srow + x*src.channels();
                uint8_t* d = drow + x*3;
                d[0]=0; d[1]=0; d[2]=0;
                switch (ch_) {
                    case Channel::R: d[0]=s[0]; break;
                    case Channel::G: d[1]=s[1]; break;
                    case Channel::B: d[2]=s[2]; break;
                }
            }
        }
        return out;
    }
};

} // namespace ops
