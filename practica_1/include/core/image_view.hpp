#pragma once
#include <cstdint>
#include <cstddef>

namespace core {

// Vista ligera a datos RGB intercalados (no-ownership)
class ImageView {
    const uint8_t* data_ = nullptr;
    int w_ = 0, h_ = 0, ch_ = 0, stride_ = 0;
public:
    ImageView() = default;
    ImageView(const uint8_t* data, int w, int h, int ch, int stride)
        : data_(data), w_(w), h_(h), ch_(ch), stride_(stride) {}

    const uint8_t* data()   const { return data_; }
    int width()   const { return w_; }
    int height()  const { return h_; }
    int channels()const { return ch_; }
    int stride()  const { return stride_; }
    bool valid()  const { return data_ && w_>0 && h_>0 && ch_>=3 && stride_>=w_*ch_; }

    // Puntero al pixel (fila y) (x) (sin checar límites)
    const uint8_t* pixel(int x,int y) const { return data_ + y*stride_ + x*ch_; }
};

} // namespace core
