
#pragma once
#include <cstdint>

namespace core {

class ImageView {
    const uint8_t* data_;
    int w_, h_, ch_, stride_;
public:
    ImageView(const uint8_t* d, int w, int h, int ch, int stride)
        : data_(d), w_(w), h_(h), ch_(ch), stride_(stride) {}
    const uint8_t* pixel(int x,int y) const { return data_ + y*stride_ + x*ch_; }
    int width() const { return w_; }
    int height() const { return h_; }
    int channels() const { return ch_; }
    int stride() const { return stride_; }
};

} // namespace core
