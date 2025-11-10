#include "SEFactory.hpp"
#include <cmath>
#include <algorithm>

namespace SEFactory {

std::vector<unsigned char> make(SEShape shape, int k){
    if(k%2==0) k+=1;
    int r = k/2;
    std::vector<unsigned char> m(k*k, 0);
    for(int y=0;y<k;++y){
        for(int x=0;x<k;++x){
            int dx = x - r;
            int dy = y - r;
            bool on=false;
            switch(shape){
                case SEShape::Square:  on = true; break;
                case SEShape::Diamond: on = (std::abs(dx)+std::abs(dy) <= r); break;
                case SEShape::Disk:    on = (dx*dx + dy*dy <= r*r); break;
            }
            m[y*k+x] = on ? 1 : 0;
        }
    }
    return m;
}

}
