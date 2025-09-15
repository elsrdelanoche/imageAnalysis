#pragma once
#include <cstdint>
#include <algorithm>

namespace core {

// RGB de 8 bits por canal
struct RGB8 { uint8_t r, g, b; };

// Empaquetado 0x00RRGGBB (para compatibilidad con ejercicios estilo Java)
inline uint32_t pack_rgb_0x00RRGGBB(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

inline RGB8 unpack_0x00RRGGBB(uint32_t p) {
    return RGB8{ uint8_t((p >> 16) & 0xFF), uint8_t((p >> 8) & 0xFF), uint8_t(p & 0xFF) };
}

// Clamp doble → uint8
inline uint8_t clamp_u8(double v) {
    if (v < 0.0)   return 0u;
    if (v > 255.0) return 255u;
    return static_cast<uint8_t>(v + 0.5);
}

} // namespace core
