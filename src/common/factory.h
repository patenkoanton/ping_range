#pragma once

#include <memory>

template<class T, class... Args> std::unique_ptr<T> factory_make_unique(Args... args);
