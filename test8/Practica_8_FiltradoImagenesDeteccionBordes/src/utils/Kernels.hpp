#pragma once
#include <vector>
#include <string>
namespace Kern {
struct KernelDesc { std::string name; std::vector<double> data; int k; double norm; };
std::vector<KernelDesc> LowPassKernels(int k_gauss=5, double sigma=1.0);
std::vector<KernelDesc> HighPassKernels(double alpha=1.0);
std::vector<KernelDesc> EdgeKernels();
std::vector<double> Gaussian(int k, double sigma);
std::vector<double> LoG(int k, double sigma);
}
