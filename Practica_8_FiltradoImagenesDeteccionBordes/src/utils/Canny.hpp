#pragma once
#include <vector>
namespace Canny {
struct Params { int k_gauss=5; double sigma=1.0; double t_low=0.1; double t_high=0.2; };
std::vector<double> Run(const std::vector<double>& img, int w, int h, const Params& p);
}
