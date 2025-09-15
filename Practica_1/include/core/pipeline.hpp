#pragma once
#include <memory>
#include <vector>
#include "filter.hpp"

namespace core {

// Encadena filtros: out_i → in_{i+1}
class Pipeline {
    std::vector<std::unique_ptr<Filter>> stages_;
public:
    template<typename F, typename... Args>
    Pipeline& add(Args&&... args) {
        stages_.emplace_back(std::make_unique<F>(std::forward<Args>(args)...));
        return *this;
    }

    ImageBuffer run(const ImageView& input) const {
        if (stages_.empty()) return ImageBuffer(); // vacío
        ImageBuffer cur = stages_.front()->apply(input);
        for (size_t i=1;i<stages_.size();++i) {
            cur = stages_[i]->apply(cur.view());
        }
        return cur;
    }
};

} // namespace core
