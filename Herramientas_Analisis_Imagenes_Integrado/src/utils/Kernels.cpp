#include "Kernels.hpp"
#include <cmath>
#include <algorithm>
namespace Kern {
static std::vector<double> box(int k){ double v=1.0/(k*k); return std::vector<double>(k*k, v); }
std::vector<double> Gaussian(int k, double s){
    int r=k/2; double sum=0; std::vector<double> G(k*k); double s2=2*s*s;
    for(int y=-r;y<=r;++y){
        for(int x=-r;x<=r;++x){
            double v = std::exp(-(x*x+y*y)/s2);
            G[(y+r)*k+(x+r)] = v; sum+=v;
        }
    }
    for(double& v: G) v/=sum; return G;
}
std::vector<double> LoG(int k, double s){
    int r=k/2; double s2=s*s; std::vector<double> L(k*k); double sum=0;
    for(int y=-r;y<=r;++y){
        for(int x=-r;x<=r;++x){
            double r2=x*x+y*y;
            double v = ((r2 - 2*s2)/(s2*s2))*std::exp(-r2/(2*s2));
            L[(y+r)*k+(x+r)] = v; sum+=v;
        }
    }
    double mean=sum/(k*k); for(double& v: L) v-=mean; return L;
}
std::vector<KernelDesc> LowPassKernels(int k_gauss, double sigma){
    return { {"Media 3x3", box(3),3,1.0}, {"Media 5x5", box(5),5,1.0}, {"Media 7x7", box(7),7,1.0},
             {"Gauss", Gaussian(k_gauss,sigma), k_gauss, 1.0} };
}
std::vector<KernelDesc> HighPassKernels(double alpha){
    std::vector<double> sharpen = { 0,-1,0,-1,5,-1,0,-1,0 };
    std::vector<double> lap4 = { 0,-1,0,-1,4,-1,0,-1,0 };
    std::vector<double> lap8 = { -1,-1,-1,-1,8,-1,-1,-1,-1 };
    return { {"Sharpen",sharpen,3,1.0},{"Laplaciano4",lap4,3,1.0},{"Laplaciano8",lap8,3,1.0} };
}
std::vector<KernelDesc> EdgeKernels(){
    std::vector<KernelDesc> ks;
    ks.push_back({"Roberts Gx",{1,0,0,-1},2,1.0});
    ks.push_back({"Roberts Gy",{0,1,-1,0},2,1.0});
    ks.push_back({"Prewitt Gx",{-1,0,1,-1,0,1,-1,0,1},3,1.0});
    ks.push_back({"Prewitt Gy",{-1,-1,-1,0,0,0,1,1,1},3,1.0});
    ks.push_back({"Sobel Gx",{-1,0,1,-2,0,2,-1,0,1},3,1.0});
    ks.push_back({"Sobel Gy",{-1,-2,-1,0,0,0,1,2,1},3,1.0});
    ks.push_back({"Scharr Gx",{-3,0,3,-10,0,10,-3,0,3},3,1.0});
    ks.push_back({"Scharr Gy",{-3,-10,-3,0,0,0,3,10,3},3,1.0});
    ks.push_back({"Kirsch N",{-3,-3,-3,-3,0,-3,5,5,5},3,1.0});
    ks.push_back({"Kirsch E",{-3,-3,5,-3,0,5,-3,-3,5},3,1.0});
    ks.push_back({"Robinson N",{-1,-1,-1,0,0,0,1,1,1},3,1.0});
    ks.push_back({"Robinson E",{-1,0,1,-1,0,1,-1,0,1},3,1.0});
    return ks;
}
}
