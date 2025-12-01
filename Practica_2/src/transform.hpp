#pragma once
#include "image_utils.hpp"

// RGB <-> CMY / CMYK
ImageCMY rgb_to_cmy(const ImageRGB &rgb);
ImageRGB cmy_to_rgb(const ImageCMY &cmy);
ImageCMYK cmy_to_cmyk(const ImageCMY &cmy);
ImageCMY cmyk_to_cmy(const ImageCMYK &cmyk);

// RGB <-> YIQ
ImageYIQ rgb_to_yiq(const ImageRGB &rgb);
ImageRGB yiq_to_rgb(const ImageYIQ &yiq);

// RGB <-> HSV
ImageHSV rgb_to_hsv(const ImageRGB &rgb);
ImageRGB hsv_to_rgb(const ImageHSV &hsv);

// RGB <-> HSI
ImageHSI rgb_to_hsi(const ImageRGB &rgb);
ImageRGB hsi_to_rgb(const ImageHSI &hsi);

// Helper (no-op)
ImageRGB rgb_from_assuming_cmy_displayed(const ImageRGB &rgb);
