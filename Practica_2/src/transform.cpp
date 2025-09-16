#include "transform.hpp"
#include <algorithm>
#include <cmath>

static inline float clamp01(float x){ return x<0.f?0.f:(x>1.f?1.f:x); }

ImageCMY rgb_to_cmy(const ImageRGB &rgb) {
    ImageCMY out; out.width=rgb.width; out.height=rgb.height;
    out.c = rgb.r; out.m = rgb.g; out.y = rgb.b;
    for (int i=0, n=rgb.width*rgb.height; i<n; ++i){
        out.c.data[i] = 1.f - rgb.r.data[i];
        out.m.data[i] = 1.f - rgb.g.data[i];
        out.y.data[i] = 1.f - rgb.b.data[i];
    }
    return out;
}

ImageRGB cmy_to_rgb(const ImageCMY &cmy) {
    ImageRGB out; out.width=cmy.width; out.height=cmy.height;
    out.r = cmy.c; out.g = cmy.m; out.b = cmy.y;
    for (int i=0, n=cmy.width*cmy.height; i<n; ++i){
        out.r.data[i] = 1.f - cmy.c.data[i];
        out.g.data[i] = 1.f - cmy.m.data[i];
        out.b.data[i] = 1.f - cmy.y.data[i];
    }
    return out;
}

ImageCMYK cmy_to_cmyk(const ImageCMY &cmy) {
    ImageCMYK out; out.width=cmy.width; out.height=cmy.height;
    out.c.width=out.m.width=out.y.width=out.k.width=cmy.width;
    out.c.height=out.m.height=out.y.height=out.k.height=cmy.height;
    out.c.data.resize(cmy.width*cmy.height);
    out.m.data.resize(cmy.width*cmy.height);
    out.y.data.resize(cmy.width*cmy.height);
    out.k.data.resize(cmy.width*cmy.height);
    for (int i=0, n=cmy.width*cmy.height; i<n; ++i){
        float C=cmy.c.data[i], M=cmy.m.data[i], Y=cmy.y.data[i];
        float K = std::min(C, std::min(M, Y));
        out.k.data[i]=K;
        if (K>=0.999f){
            out.c.data[i]=out.m.data[i]=out.y.data[i]=0.f;
        } else {
            out.c.data[i]=(C-K)/(1.f-K);
            out.m.data[i]=(M-K)/(1.f-K);
            out.y.data[i]=(Y-K)/(1.f-K);
        }
    }
    return out;
}

ImageCMY cmyk_to_cmy(const ImageCMYK &cmyk) {
    ImageCMY out; out.width=cmyk.width; out.height=cmyk.height;
    out.c.width=out.m.width=out.y.width=cmyk.width;
    out.c.height=out.m.height=out.y.height=cmyk.height;
    out.c.data.resize(cmyk.width*cmyk.height);
    out.m.data.resize(cmyk.width*cmyk.height);
    out.y.data.resize(cmyk.width*cmyk.height);
    for (int i=0, n=cmyk.width*cmyk.height; i<n; ++i){
        float C=cmyk.c.data[i], M=cmyk.m.data[i], Y=cmyk.y.data[i], K=cmyk.k.data[i];
        out.c.data[i] = C*(1.f-K) + K;
        out.m.data[i] = M*(1.f-K) + K;
        out.y.data[i] = Y*(1.f-K) + K;
    }
    return out;
}

ImageYIQ rgb_to_yiq(const ImageRGB &rgb) {
    ImageYIQ out; out.width=rgb.width; out.height=rgb.height;
    out.y.width=out.i.width=out.q.width=rgb.width;
    out.y.height=out.i.height=out.q.height=rgb.height;
    out.y.data.resize(rgb.width*rgb.height);
    out.i.data.resize(rgb.width*rgb.height);
    out.q.data.resize(rgb.width*rgb.height);
    for (int idx=0, n=rgb.width*rgb.height; idx<n; ++idx){
        float R=rgb.r.data[idx], G=rgb.g.data[idx], B=rgb.b.data[idx];
        float Y = 0.299f*R + 0.587f*G + 0.114f*B;
        float I = 0.596f*R - 0.274f*G - 0.322f*B;
        float Q = 0.211f*R - 0.523f*G + 0.312f*B;
        // normalize I,Q roughly to 0..1 for display
        out.y.data[idx] = clamp01(Y);
        out.i.data[idx] = clamp01((I + 0.596f) / (2*0.596f));
        out.q.data[idx] = clamp01((Q + 0.523f) / (2*0.523f));
    }
    return out;
}

ImageRGB yiq_to_rgb(const ImageYIQ &yiq) {
    ImageRGB out; out.width=yiq.width; out.height=yiq.height;
    out.r.width=out.g.width=out.b.width=yiq.width;
    out.r.height=out.g.height=out.b.height=yiq.height;
    out.r.data.resize(yiq.width*yiq.height);
    out.g.data.resize(yiq.width*yiq.height);
    out.b.data.resize(yiq.width*yiq.height);
    for (int idx=0, n=yiq.width*yiq.height; idx<n; ++idx){
        float Y=yiq.y.data[idx];
        float I=(yiq.i.data[idx] * (2*0.596f)) - 0.596f;
        float Q=(yiq.q.data[idx] * (2*0.523f)) - 0.523f;
        float R = Y + 0.956f*I + 0.621f*Q;
        float G = Y - 0.272f*I - 0.647f*Q;
        float B = Y - 1.106f*I + 1.703f*Q;
        out.r.data[idx]=clamp01(R);
        out.g.data[idx]=clamp01(G);
        out.b.data[idx]=clamp01(B);
    }
    return out;
}

ImageHSV rgb_to_hsv(const ImageRGB &rgb) {
    ImageHSV out; out.width=rgb.width; out.height=rgb.height;
    out.h.width=out.s.width=out.v.width=rgb.width;
    out.h.height=out.s.height=out.v.height=rgb.height;
    out.h.data.resize(rgb.width*rgb.height);
    out.s.data.resize(rgb.width*rgb.height);
    out.v.data.resize(rgb.width*rgb.height);
    for (int idx=0, n=rgb.width*rgb.height; idx<n; ++idx){
        float R=rgb.r.data[idx], G=rgb.g.data[idx], B=rgb.b.data[idx];
        float maxv = std::max({R,G,B});
        float minv = std::min({R,G,B});
        float V = maxv;
        float S = (maxv<=0.f)?0.f: (maxv - minv)/maxv;
        float H = 0.f;
        if (maxv != minv){
            if (maxv == R) H = (G - B) / (maxv - minv) + (G < B ? 6.f : 0.f);
            else if (maxv == G) H = (B - R) / (maxv - minv) + 2.f;
            else H = (R - G) / (maxv - minv) + 4.f;
            H /= 6.f; // normalize to 0..1
        }
        out.h.data[idx]=clamp01(H);
        out.s.data[idx]=clamp01(S);
        out.v.data[idx]=clamp01(V);
    }
    return out;
}

ImageRGB hsv_to_rgb(const ImageHSV &hsv) {
    ImageRGB out; out.width=hsv.width; out.height=hsv.height;
    out.r.width=out.g.width=out.b.width=hsv.width;
    out.r.height=out.g.height=out.b.height=hsv.height;
    out.r.data.resize(hsv.width*hsv.height);
    out.g.data.resize(hsv.width*hsv.height);
    out.b.data.resize(hsv.width*hsv.height);

    for (int idx=0, n=hsv.width*hsv.height; idx<n; ++idx){
        float H=hsv.h.data[idx]; // 0..1
        float S=hsv.s.data[idx];
        float V=hsv.v.data[idx];
        float R=V, G=V, B=V;
        if (S > 0.f){
            float h = H * 6.f;
            int i = (int)std::floor(h);
            float f = h - i;
            float p = V * (1.f - S);
            float q = V * (1.f - S * f);
            float t = V * (1.f - S * (1.f - f));
            switch (i % 6) {
                case 0: R = V; G = t; B = p; break;
                case 1: R = q; G = V; B = p; break;
                case 2: R = p; G = V; B = t; break;
                case 3: R = p; G = q; B = V; break;
                case 4: R = t; G = p; B = V; break;
                case 5: R = V; G = p; B = q; break;
            }
        }
        out.r.data[idx]=clamp01(R);
        out.g.data[idx]=clamp01(G);
        out.b.data[idx]=clamp01(B);
    }
    return out;
}

ImageHSI rgb_to_hsi(const ImageRGB &rgb) {
    ImageHSI out; out.width=rgb.width; out.height=rgb.height;
    out.h.width=out.s.width=out.i.width=rgb.width;
    out.h.height=out.s.height=out.i.height=rgb.height;
    out.h.data.resize(rgb.width*rgb.height);
    out.s.data.resize(rgb.width*rgb.height);
    out.i.data.resize(rgb.width*rgb.height);
    const float PI = 3.14159265358979323846f;
    for (int idx=0, n=rgb.width*rgb.height; idx<n; ++idx){
        float R=rgb.r.data[idx], G=rgb.g.data[idx], B=rgb.b.data[idx];
        float I = (R+G+B)/3.f;
        float minv = std::min({R,G,B});
        float S = (I>0.f) ? 1.f - (minv / I) : 0.f;
        float num = 0.5f*((R-G) + (R-B));
        float den = std::sqrt((R-G)*(R-G) + (R-B)*(G-B)) + 1e-8f;
        float theta = std::acos(std::max(-1.f, std::min(1.f, num/den)));
        float H = (B <= G) ? theta : (2.f*PI - theta);
        H /= (2.f*PI); // normalize 0..1
        out.h.data[idx]=clamp01(H);
        out.s.data[idx]=clamp01(S);
        out.i.data[idx]=clamp01(I);
    }
    return out;
}

ImageRGB hsi_to_rgb(const ImageHSI &hsi) {
    ImageRGB out; out.width=hsi.width; out.height=hsi.height;
    out.r.width=out.g.width=out.b.width=hsi.width;
    out.r.height=out.g.height=out.b.height=hsi.height;
    out.r.data.resize(hsi.width*hsi.height);
    out.g.data.resize(hsi.width*hsi.height);
    out.b.data.resize(hsi.width*hsi.height);
    const float PI = 3.14159265358979323846f;
    for (int idx=0, n=hsi.width*hsi.height; idx<n; ++idx){
        float H = hsi.h.data[idx] * 2.f * PI; // radians
        float S = hsi.s.data[idx];
        float I = hsi.i.data[idx];
        float R, G, B;
        if (S==0.f){
            R=G=B=I;
        } else {
            if (H < 2.f*PI/3.f){
                float cosH = std::cos(H);
                float cosDen = std::cos(PI/3.f - H) + 1e-8f;
                B = I*(1.f - S);
                R = I*(1.f + (S*cosH)/cosDen);
                G = 3.f*I - (R + B);
            } else if (H < 4.f*PI/3.f){
                float Hp = H - 2.f*PI/3.f;
                float cosH = std::cos(Hp);
                float cosDen = std::cos(PI/3.f - Hp) + 1e-8f;
                R = I*(1.f - S);
                G = I*(1.f + (S*cosH)/cosDen);
                B = 3.f*I - (R + G);
            } else {
                float Hp = H - 4.f*PI/3.f;
                float cosH = std::cos(Hp);
                float cosDen = std::cos(PI/3.f - Hp) + 1e-8f;
                G = I*(1.f - S);
                B = I*(1.f + (S*cosH)/cosDen);
                R = 3.f*I - (G + B);
            }
        }
        out.r.data[idx]=clamp01(R);
        out.g.data[idx]=clamp01(G);
        out.b.data[idx]=clamp01(B);
    }
    return out;
}

// No-op
ImageRGB rgb_from_assuming_cmy_displayed(const ImageRGB &rgb){ return rgb; }
