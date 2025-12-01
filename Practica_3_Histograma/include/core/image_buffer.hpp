
#pragma once
#include <vector>
#include <cstdint>
#include "image_view.hpp"

namespace core {

class ImageBuffer {
    int w_=0,h_=0,ch_=3, stride_=0;
    std::vector<uint8_t> buf_;
public:
    ImageBuffer() = default;
    ImageBuffer(int w,int h,int ch=3){ reset(w,h,ch); }

    void reset(int w,int h,int ch=3){
        w_=w; h_=h; ch_=ch; stride_=w_*ch_;
        buf_.assign(h_*stride_, 0);
    }
    uint8_t* data(){ return buf_.data(); }
    const uint8_t* data() const { return buf_.data(); }
    int width() const { return w_; }
    int height() const { return h_; }
    int channels() const { return ch_; }
    int stride() const { return stride_; }
    bool valid() const { return !buf_.empty() && ch_>=3; }
    ImageView view() const { return ImageView(buf_.data(), w_, h_, ch_, stride_); }
};

} // namespace core
