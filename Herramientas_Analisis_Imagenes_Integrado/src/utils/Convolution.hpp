#pragma once
#include <vector>
namespace Conv {
std::vector<double> Convolve(const std::vector<double>& src, int w, int h,
                             const std::vector<double>& kernel, int k);
std::vector<double> GradMag(const std::vector<double>& gx, const std::vector<double>& gy);
std::vector<double> NonMaxSupp(const std::vector<double>& mag, const std::vector<double>& ang, int w, int h);
}
