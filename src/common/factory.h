#pragma once

#include <memory>

namespace Factory {
    template<class T, class... Args> std::unique_ptr<T> make_unique(Args... args);
    template<class T, class... Args> std::shared_ptr<T> make_shared(Args... args);
}
