#include "FFT.hpp"
#include <cmath>

namespace FFT {

static unsigned reverse_bits(unsigned x, int n){
    unsigned r=0;
    for(int i=0;i<n;++i){ r=(r<<1) | (x&1u); x>>=1; }
    return r;
}

void fft1d(std::vector<cd>& a, int dir){
    const size_t n = a.size();
    int lg = 0; while((1u<<lg) < n) ++lg;

    // Bit-reversal
    for(size_t i=0;i<n;++i){
        size_t j = reverse_bits((unsigned)i, lg);
        if(i<j) std::swap(a[i], a[j]);
    }

    const double PI = std::acos(-1.0);
    for(size_t len=2; len<=n; len<<=1){
        double ang = 2*PI/len * (dir>0? -1 : 1); // dir: +1 FFT, -1 IFFT
        cd wlen = std::polar(1.0, ang);
        for(size_t i=0; i<n; i+=len){
            cd w = 1.0;
            for(size_t j=0;j<len/2;++j){
                cd u = a[i+j];
                cd v = a[i+j+len/2] * w;
                a[i+j] = u+v;
                a[i+j+len/2] = u-v;
                w *= wlen;
            }
        }
    }
    if(dir<0){
        for(size_t i=0;i<n;++i) a[i] /= (double)n;
    }
}

void fft2d(std::vector<cd>& img, int N, int dir){
    // Rows
    std::vector<cd> temp(N);
    for(int y=0;y<N;++y){
        for(int x=0;x<N;++x) temp[x] = img[y*N+x];
        fft1d(temp, dir);
        for(int x=0;x<N;++x) img[y*N+x] = temp[x];
    }
    // Cols
    for(int x=0;x<N;++x){
        for(int y=0;y<N;++y) temp[y] = img[y*N+x];
        fft1d(temp, dir);
        for(int y=0;y<N;++y) img[y*N+x] = temp[y];
    }
    // Global normalization already done by two passes when dir<0 (1/N then 1/N), so scale by 1/N again:
    if(dir<0){
        double invN = 1.0; // already normalized fully by two 1D ifft passes
        (void)invN;
    }
}

void fftshift2d(std::vector<cd>& F, int N){
    int h = N/2;
    for(int y=0;y<h;++y){
        for(int x=0;x<h;++x){
            std::swap(F[y*N+x], F[(y+h)*N + (x+h)]); // I <-> III
            std::swap(F[y*N + (x+h)], F[(y+h)*N + x]); // II <-> IV
        }
    }
}

void mag_phase(const std::vector<cd>& F, int N, std::vector<double>& mag, std::vector<double>& phase){
    mag.resize(N*N);
    phase.resize(N*N);
    for(int i=0;i<N*N;++i){
        mag[i] = std::abs(F[i]);
        phase[i] = std::arg(F[i]);
    }
}

} // namespace
