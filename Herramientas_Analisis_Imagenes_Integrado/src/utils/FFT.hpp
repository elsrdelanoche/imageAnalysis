#pragma once
#include <complex>
#include <vector>

namespace FFT {

using cd = std::complex<double>;

// In-place radix-2 Cooley–Tukey 1D FFT. dir=+1 for FFT, dir=-1 for IFFT (unnormalized)
void fft1d(std::vector<cd>& a, int dir);

// 2D FFT via separability on N×N image (row-major). dir=+1 FFT, dir=-1 IFFT (scales by 1/N^2 when dir=-1).
void fft2d(std::vector<cd>& img, int N, int dir);

// Shift quadrants to center low frequencies
void fftshift2d(std::vector<cd>& F, int N);

// Extract magnitude and phase
void mag_phase(const std::vector<cd>& F, int N, std::vector<double>& mag, std::vector<double>& phase);

} // namespace
